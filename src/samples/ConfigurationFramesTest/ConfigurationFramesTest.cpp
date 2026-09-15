//******************************************************************************************************
//  ConfigurationFrames.cpp - Gbtc
//
//  Copyright © 2026, Grid Protection Alliance.  All Rights Reserved.
//
//  Licensed to the Grid Protection Alliance (GPA) under one or more contributor license agreements. See
//  the NOTICE file distributed with this work for additional information regarding copyright ownership.
//  The GPA licenses this file to you under the MIT License (MIT), the "License"; you may
//  not use this file except in compliance with the License. You may obtain a copy of the License at:
//
//      http://opensource.org/licenses/MIT
//
//  Unless agreed to in writing, the subject software distributed under the License is distributed on an
//  "AS-IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. Refer to the
//  License for the specific language governing permissions and limitations.
//
//  Code Modification History:
//  ----------------------------------------------------------------------------------------------------
//  09/15/2026 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "../../lib/transport/SubscriberInstance.h"
#include "../../lib/Convert.h"
#include <boost/iostreams/device/back_inserter.hpp>
#include "../../lib/EndianConverter.h"
#include "../../lib/transport/CompactMeasurement.h"
#include "../../lib/transport/DataPublisher.h"
#include "../../lib/transport/tssc/TSSCEncoder.h"
#include "../../lib/transport/tssc/TSSCDecoder.h"
#include <limits>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace sttp;
using namespace sttp::transport;

static std::string GuidText(uint64_t id)
{
	char buffer[40];
	std::snprintf(buffer, sizeof(buffer), "00000000-0000-0000-0000-%012llx", static_cast<unsigned long long>(id));
	return buffer;
}

static Guid GuidValue(uint64_t id) { return ParseGuid(GuidText(id).c_str()); }

struct Shape
{
	std::string kind;
	unsigned devices, points, stride;
	bool gzip;
};

static std::vector<uint8_t> Metadata(const Shape& shape, size_t& xmlBytes)
{
	std::ostringstream xml;
	xml << "<NewDataSet>";

	for (unsigned d = 0; d < shape.devices; ++d)
		xml << "<DeviceDetail><Acronym>D" << d << "</Acronym><Name>Test device</Name><UniqueID>"
		<< GuidText(10000000 + d) << "</UniqueID><FramesPerSecond>30</FramesPerSecond>"
		<< "<UpdatedOn>2026-09-15T12:34:56.789Z</UpdatedOn></DeviceDetail>";
	
	uint64_t id = 0;
	
	for (unsigned d = 0; d < shape.devices; ++d)
	{
		for (unsigned p = 1; p <= shape.points; ++p)
		{
			for (unsigned part = 0; part < (shape.kind == "phasor" ? 2U : 1U); ++part)
			{
				const char* suffix = shape.kind == "analog" ? "AV" : shape.kind == "digital" ? "DV" : part == 0 ? "PA" : "PM";
				++id;
				xml << "<MeasurementDetail><DeviceAcronym>D" << d << "</DeviceAcronym><ID>TEST:" << id
					<< "</ID><SignalID>" << GuidText(id) << "</SignalID><PointTag>TEST_" << id
					<< "</PointTag><SignalReference>D" << d << '-' << suffix << p * shape.stride
					<< "</SignalReference><PhasorSourceIndex>" << (shape.kind == "phasor" ? p * shape.stride : 0)
					<< "</PhasorSourceIndex><Description>Synthetic measurement for metadata scaling test</Description>"
					<< "<UpdatedOn>2026-09-15T12:34:56.789Z</UpdatedOn></MeasurementDetail>";
			}
		}
	}

	if (shape.kind == "phasor")
	{
		for (unsigned d = 0; d < shape.devices; ++d)
		{
			for (unsigned p = 1; p <= shape.points; ++p)
				xml << "<PhasorDetail><DeviceAcronym>D" << d << "</DeviceAcronym><Label>Test phasor</Label>"
				<< "<Type>V</Type><Phase>+</Phase><SourceIndex>" << p * shape.stride
				<< "</SourceIndex><UpdatedOn>2026-09-15T12:34:56.789Z</UpdatedOn></PhasorDetail>";
		}
	}

	xml << "</NewDataSet>";
	const auto text = xml.str();
	xmlBytes = text.size();
	
	if (!shape.gzip) 
		return { text.begin(), text.end() };
	
	std::vector<char> compressed;
	{
		boost::iostreams::filtering_ostream output;
		output.push(boost::iostreams::gzip_compressor());
		output.push(boost::iostreams::back_inserter(compressed));
		output.write(text.data(), static_cast<std::streamsize>(text.size()));
	}
	
	return { compressed.begin(), compressed.end() };
}

class TestSubscriber final : public SubscriberInstance
{
public:
	void Parse(const std::vector<uint8_t>& payload) { ReceivedMetadata(payload); }
	bool parsed = false;
protected:
	void StatusMessage(const std::string&) override {}
	void ErrorMessage(const std::string& message) override { throw std::runtime_error(message); }
	void ParsedMetadata() override { parsed = true; }
};

static void Require(bool condition, const char* message)
{
	if (!condition)
		throw std::runtime_error(message);
}

static void Validate(TestSubscriber& subscriber, const Shape& shape)
{
	Require(subscriber.parsed, "ParsedMetadata was not called");
	
	size_t measured = 0, frames = 0;
	const unsigned parts = shape.kind == "phasor" ? 2U : 1U;
	subscriber.IterateMeasurementMetadata([&](MeasurementMetadataPtr, void*) { ++measured; }, nullptr);
	
	Require(measured == size_t(shape.devices) * shape.points * parts, "Measurement count mismatch");
	
	subscriber.IterateConfigurationFrames([&](ConfigurationFramePtr frame, void*)
	{
		++frames;
		const auto d = std::stoul(frame->DeviceAcronym.substr(1));
		const unsigned slots = shape.points * shape.stride;
		
		Require(frame->Measurements.size() == size_t(shape.points) * parts, "Frame signal count mismatch");
		
		if (shape.kind == "phasor")
		{
			Require(frame->Phasors.size() == slots, "Phasor slot count mismatch");
			
			for (unsigned i = 1; i <= slots; ++i)
			{
				const auto& phasor = frame->Phasors[i - 1];
				Require(phasor->Phasor->SourceIndex == i, "Phasor index mismatch");
				
				if (i % shape.stride == 0)
				{
					const uint64_t id = (uint64_t(d) * shape.points + i / shape.stride - 1) * 2 + 1;
					Require(phasor->Angle && phasor->Magnitude, "Missing phasor component");
					Require(phasor->Angle->SignalID == GuidValue(id), "Wrong phasor angle");
					Require(phasor->Magnitude->SignalID == GuidValue(id + 1), "Wrong phasor magnitude");
				}
				else
				{
					Require(!phasor->Angle && !phasor->Magnitude && phasor->Phasor->Label == "UNDEFINED", "Wrong phasor placeholder");
				}
			}
		}
		else
		{
			const auto& values = shape.kind == "analog" ? frame->Analogs : frame->Digitals;
			
			Require(values.size() == slots, "Scalar slot count mismatch");
			
			for (unsigned i = 1; i <= slots; ++i)
			{
				Require(values[i - 1]->Reference.Index == i, "Scalar index mismatch");
				
				if (i % shape.stride == 0)
					Require(values[i - 1]->SignalID == GuidValue(uint64_t(d) * shape.points + i / shape.stride), "Wrong scalar signal");
				else 
					Require(values[i - 1]->SignalID == Empty::Guid && values[i - 1]->PointTag == "UNDEFINED", "Wrong scalar placeholder");
			}
		}
	}, nullptr);

	Require(frames == shape.devices, "Device/frame count mismatch");
}

static void Run(const Shape& shape)
{
	size_t xmlBytes = 0;
	const auto payload = Metadata(shape, xmlBytes);
	TestSubscriber subscriber;
	
	subscriber.SetMetadataCompressed(shape.gzip);
	subscriber.Parse(payload);
	
	Validate(subscriber, shape);
	
	std::cout << "RESULT," << shape.kind << ',' << shape.devices << ',' << shape.points << ',' << shape.stride
		<< ',' << shape.gzip << ',' << xmlBytes << ',' << payload.size() << ",PASS" << std::endl;
}

static void DuplicateRegression(const std::string& kind)
{
	const Shape shape{ kind, 1, 1, 1, false };
	size_t size;
	const auto payload = Metadata(shape, size);
	
	std::string xml(payload.begin(), payload.end());
	const std::string suffix = kind == "analog" ? "AV" : kind == "digital" ? "DV" : "PA";
	
	std::string extra = "<MeasurementDetail><DeviceAcronym>D0</DeviceAcronym><ID>TEST:999</ID><SignalID>" + GuidText(999) +
		"</SignalID><PointTag>DUPLICATE</PointTag><SignalReference>D0-" + suffix +
		"1</SignalReference><PhasorSourceIndex>1</PhasorSourceIndex><UpdatedOn>2026-09-15T12:34:56.789Z</UpdatedOn></MeasurementDetail>";
	
	if (kind == "phasor")
		extra += "<PhasorDetail><DeviceAcronym>D0</DeviceAcronym><Label>DUPLICATE</Label><Type>V</Type><Phase>+</Phase><SourceIndex>1</SourceIndex><UpdatedOn>2026-09-15T12:34:56.789Z</UpdatedOn></PhasorDetail>";
	
	xml.insert(xml.rfind("</NewDataSet>"), extra);
	
	TestSubscriber subscriber;
	
	subscriber.SetMetadataCompressed(false);
	subscriber.Parse({ xml.begin(), xml.end() });
	
	ConfigurationFramePtr frame;
	
	Require(subscriber.TryGetConfigurationFrame("D0", frame), "Missing duplicate regression frame");
	
	if (kind == "phasor")
	{
		Require(frame->Phasors[0]->Phasor->Label == "Test phasor" && frame->Phasors[0]->Angle->SignalID == GuidValue(1), "First phasor match changed");
	}
	else
	{
		const auto& values = kind == "analog" ? frame->Analogs : frame->Digitals;
		Require(values[0]->SignalID == GuidValue(1), "First scalar match changed");
	}

	std::cout << "REGRESSION," << kind << ",duplicate,PASS\n";
}

static void WideIndexRegression()
{
	for (const int32_t index : {65535, 65536, 100000, std::numeric_limits<int32_t>::max()})
	{
		const std::string text = "D0-AV" + std::to_string(index);
		const SignalReference reference(text);
		Require(reference.Index == index, "32-bit signal-reference parsing failed");
		std::ostringstream formatted;
		formatted << reference;
		Require(formatted.str() == text, "32-bit signal-reference formatting failed");
	}
	Require(SignalReference("D0-AV2147483648").Index == 0, "Overflowed reference did not retain default index");

	// Build a genuine signal-index-cache wire payload containing over 65535 entries.
	std::vector<uint8_t> wire;
	const Guid subscriberID = GuidValue(123456789);
	WriteBytes(wire, uint32_t(0));
	WriteBytes(wire, subscriberID);
	constexpr uint32_t entries = 70001;
	EndianConverter::WriteBigEndianBytes(wire, entries);
	for (uint32_t i = 0; i < entries; ++i)
	{
		const int32_t index = i == entries - 1 ? std::numeric_limits<int32_t>::max() : static_cast<int32_t>(65535 + i);
		EndianConverter::WriteBigEndianBytes(wire, index);
		WriteBytes(wire, GuidValue(uint64_t(index) + 1));
		EndianConverter::WriteBigEndianBytes(wire, uint32_t(4));
		for (const char c : std::string("TEST")) wire.push_back(static_cast<uint8_t>(c));
		EndianConverter::WriteBigEndianBytes(wire, uint64_t(index));
	}
	EndianConverter::WriteBigEndianBytes(wire, uint32_t(0));
	const uint32_t length = EndianConverter::Default.ConvertBigEndian(static_cast<uint32_t>(wire.size()));
	std::memcpy(wire.data(), &length, sizeof(length));
	const auto cache = NewSharedPtr<SignalIndexCache>();
	Guid decodedSubscriber;
	cache->Decode(wire, decodedSubscriber);
	Require(decodedSubscriber == subscriberID && cache->Count() == entries, "Large wire cache count/identity mismatch");
	CompactMeasurement codec(cache, nullptr, false);
	for (const int32_t index : {65535, 65536, 100000, std::numeric_limits<int32_t>::max()})
	{
		const auto id = GuidValue(uint64_t(index) + 1);
		Require(cache->GetSignalID(index) == id && cache->GetSignalIndex(id) == index, "Wide cache lookup failed");
		Measurement input;
		input.SignalID = id;
		input.Value = 12.5;
		input.Multiplier = 1.0;
		std::vector<uint8_t> bytes;
		codec.SerializeMeasurement(input, bytes, index);
		uint32_t offset = 0;
		MeasurementPtr output;
		Require(codec.TryParseMeasurement(bytes.data(), offset, static_cast<uint32_t>(bytes.size()), output), "Wide compact measurement decode failed");
		Require(offset == bytes.size() && output->SignalID == id && output->Value == input.Value && output->ID == uint64_t(index), "Wide compact measurement mismatch");
	}
	std::cout << "REGRESSION,int32,70001-entry-wire-cache-and-compact-roundtrip,PASS\n";
}
static void PublisherIndexRegression()
{
	const auto device = NewSharedPtr<DeviceMetadata>();
	device->Acronym = "D0";
	device->Name = "Wide index regression";
	device->UniqueID = GuidValue(987654);
	device->FramesPerSecond = 30;
	device->UpdatedOn = UtcNow();
	std::vector<MeasurementMetadataPtr> measurements;
	std::vector<PhasorMetadataPtr> phasors;
	for (const int32_t index : {34464, 100000})
	{
		const auto phasor = NewSharedPtr<PhasorMetadata>();
		phasor->DeviceAcronym = "D0";
		phasor->Label = "Test";
		phasor->SourceIndex = index;
		phasor->Type = index == 100000 ? "V" : "I";
		phasor->Phase = "+";
		phasor->UpdatedOn = UtcNow();
		phasors.push_back(phasor);
		const auto measurement = NewSharedPtr<MeasurementMetadata>();
		measurement->DeviceAcronym = "D0";
		measurement->ID = "TEST:" + std::to_string(index);
		measurement->PointTag = "TEST_" + std::to_string(index);
		measurement->SignalID = GuidValue(index);
		measurement->Reference = SignalReference("D0-PA" + std::to_string(index));
		measurement->PhasorSourceIndex = index;
		measurement->UpdatedOn = UtcNow();
		measurements.push_back(measurement);
	}
	DataPublisher publisher;
	publisher.DefineMetadata({device}, measurements, phasors);
	const auto voltage = publisher.FilterMetadata("FILTER MeasurementDetail WHERE SignalAcronym = 'VPHA'");
	const auto current = publisher.FilterMetadata("FILTER MeasurementDetail WHERE SignalAcronym = 'IPHA'");
	Require(voltage.size() == 1 && current.size() == 1, "Publisher phasor index collision");
	Require(voltage[0]->PhasorSourceIndex == 100000 && voltage[0]->Reference.Index == 100000, "Publisher truncated 32-bit metadata index");
	Require(current[0]->PhasorSourceIndex == 34464, "Publisher confused indexes differing by 65536");
	std::cout << "REGRESSION,int32,publisher-index-roundtrip-and-collision,PASS\n";
}

static void TSSCIndexRegression()
{
	using namespace sttp::transport::tssc;
	std::vector<uint8_t> bytes(4096);
	TSSCEncoder encoder;
	encoder.SetBuffer(bytes.data(), 0, static_cast<uint32_t>(bytes.size()));
	const std::vector<int32_t> indexes{65535, 65536, 100000, 0, 100000};
	for (const auto index : indexes)
		Require(encoder.TryAddMeasurement(index, 123456789, 0, 12.5f), "TSSC encode failed");
	const auto length = encoder.FinishBlock();
	TSSCDecoder decoder;
	decoder.SetBuffer(bytes.data(), 0, length);
	for (const auto expected : indexes)
	{
		int32_t index;
		int64_t timestamp;
		uint32_t quality;
		float32_t value;
		Require(decoder.TryGetMeasurement(index, timestamp, quality, value), "TSSC decode failed");
		Require(index == expected && timestamp == 123456789 && quality == 0 && value == 12.5f, "TSSC wide index mismatch");
	}
	std::cout << "REGRESSION,int32,TSSC-wide-index-roundtrip,PASS\n";
}

int main()
{
	try
	{
		for (const auto& kind : { "analog", "digital", "phasor" })
		{
			Run({ kind, 3, 10, 1, false });
			Run({ kind, 2, 3, 7, true });
			Run({ kind, 1, 1, 65535, false });
			Run({ kind, 1, 1, 65536, false });
			Run({ kind, 1, 1, 100000, true });
			DuplicateRegression(kind);
		}
		
		Run({ "analog", 1, 70000, 1, true });
		WideIndexRegression();
		PublisherIndexRegression();
		TSSCIndexRegression();

		std::cout << "All configuration-frame regressions passed.\n";
		return 0;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "FAIL: " << ex.what() << std::endl;
		return 1;
	}
}
