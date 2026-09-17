//******************************************************************************************************
//  PublisherCacheIndexTest.cpp - Gbtc
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
//  09/17/2026 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

// Regression test for the DataPacketFlags::CacheIndex flag on published data packets.
//
// For STTP v2 and later the publisher alternates signal index cache indexes: the first cache of a
// connection is index 0, the cache sent for the next subscription is index 1, then 0 again, and so on.
// Subscribers pick the cache used to decode a packet from the CacheIndex flag in the data packet
// header, so a publisher that never sets that flag leaves every subscriber decoding run-time IDs with
// the cache from the PREVIOUS subscription once the active index reaches 1. Run-time IDs are dense and
// start at zero in both caches, so nothing fails loudly -- measurements are simply attributed to the
// wrong signal IDs.
//
// The test starts a publisher with two disjoint sets of signals, subscribes to set A, resubscribes to
// set B (which makes cache index 1 active), then resubscribes to set A again (back to index 0). Each
// published value encodes its own signal, so any packet decoded against the wrong cache is caught as a
// signal ID that does not match its value. Both the compact and TSSC payload formats are covered.

#include "../../lib/transport/PublisherInstance.h"
#include "../../lib/transport/SubscriberInstance.h"
#include "../../lib/Convert.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace sttp;
using namespace sttp::transport;
using namespace std::chrono_literals;

// Signals 1 - 4 make up set A, signals 5 - 8 make up set B
static constexpr int32_t SignalsPerSet = 4;
static constexpr int32_t TotalSignals = SignalsPerSet * 2;

static void Require(const bool condition, const std::string& message)
{
	if (!condition)
		throw std::runtime_error(message);
}

static Guid SignalID(const int32_t ordinal)
{
	char buffer[40];
	std::snprintf(buffer, sizeof(buffer), "00000000-0000-0000-0000-%012d", ordinal);
	return ParseGuid(buffer);
}

// Each signal publishes a distinct, exactly representable value that identifies the signal it belongs
// to, which is what turns a cache mix-up into a detectable mismatch rather than plausible-looking data.
static float64_t SignalValue(const int32_t ordinal)
{
	return 1000.0 + ordinal;
}

static int32_t OrdinalOf(const Guid& signalID)
{
	for (int32_t ordinal = 1; ordinal <= TotalSignals; ordinal++)
	{
		if (SignalID(ordinal) == signalID)
			return ordinal;
	}

	return -1;
}

// Set A is the first half of the signals, set B the second half - the sets are disjoint but their
// run-time ID ranges overlap exactly, which is what makes a stale cache silently mis-attribute data.
static std::set<int32_t> SetOrdinals(const bool setB)
{
	std::set<int32_t> ordinals;

	for (int32_t i = 1; i <= SignalsPerSet; i++)
		ordinals.insert(setB ? SignalsPerSet + i : i);

	return ordinals;
}

static std::string FilterExpression(const bool setB)
{
	return std::string("FILTER ActiveMeasurements WHERE PointTag LIKE 'TEST_") + (setB ? 'B' : 'A') + "_%'";
}

class TestPublisher final : public PublisherInstance
{
protected:
	void StatusMessage(const std::string&) override {}
	void ErrorMessage(const std::string&) override {}
};

class TestSubscriber final : public SubscriberInstance
{
private:
	mutable std::mutex m_lock;
	std::vector<std::pair<Guid, float64_t>> m_received;
	std::string m_error;

protected:
	void StatusMessage(const std::string&) override {}

	void ErrorMessage(const std::string& message) override
	{
		std::lock_guard<std::mutex> lock(m_lock);

		if (m_error.empty())
			m_error = message;
	}

	void SubscriptionUpdated(const SignalIndexCachePtr&) override { ++Updates; }

	// Keep teardown quiet so test output carries only test results
	void ConnectionEstablished() override {}
	void ConnectionTerminated() override {}

	void ReceivedNewMeasurements(const std::vector<MeasurementPtr>& measurements) override
	{
		std::lock_guard<std::mutex> lock(m_lock);

		for (const auto& measurement : measurements)
			m_received.emplace_back(measurement->SignalID, measurement->Value);
	}

public:
	std::atomic<int32_t> Updates { 0 };

	// Returns the ordinals seen since the last call to Clear, throwing if any measurement carried a
	// value that does not belong to the signal ID it was reported under - i.e., it was decoded with the
	// wrong signal index cache.
	std::set<int32_t> TakeOrdinals()
	{
		std::vector<std::pair<Guid, float64_t>> received;
		std::string error;

		{
			std::lock_guard<std::mutex> lock(m_lock);
			received = m_received;
			error = m_error;
		}

		Require(error.empty(), "Subscriber reported an error: " + error);

		std::set<int32_t> ordinals;

		for (const auto& [signalID, value] : received)
		{
			const int32_t ordinal = OrdinalOf(signalID);

			Require(ordinal > 0, "Received unknown signal ID " + ToString(signalID));
			Require(value == SignalValue(ordinal), "Signal " + ToString(signalID) + " reported value " + ToString(value) +
				", expected " + ToString(SignalValue(ordinal)) + " - packet was decoded with the wrong signal index cache");

			ordinals.insert(ordinal);
		}

		return ordinals;
	}

	void Clear()
	{
		std::lock_guard<std::mutex> lock(m_lock);
		m_received.clear();
	}
};

static void DefineMetadata(TestPublisher& publisher)
{
	const datetime_t timestamp = UtcNow();

	const DeviceMetadataPtr device = NewSharedPtr<DeviceMetadata>();
	device->Name = "Cache Index Test PMU";
	device->Acronym = "CACHETEST";
	device->UniqueID = SignalID(999);
	device->FramesPerSecond = 30;
	device->ProtocolName = "STTP";
	device->UpdatedOn = timestamp;

	std::vector<MeasurementMetadataPtr> measurements;

	for (int32_t ordinal = 1; ordinal <= TotalSignals; ordinal++)
	{
		const bool setB = ordinal > SignalsPerSet;
		const int32_t index = setB ? ordinal - SignalsPerSet : ordinal;

		const MeasurementMetadataPtr measurement = NewSharedPtr<MeasurementMetadata>();
		measurement->ID = "TEST:" + ToString(ordinal);
		measurement->PointTag = std::string("TEST_") + (setB ? 'B' : 'A') + "_" + ToString(index);
		measurement->SignalID = SignalID(ordinal);
		measurement->DeviceAcronym = device->Acronym;
		measurement->Reference = SignalReference(device->Acronym + "-AV" + ToString(ordinal));
		measurement->UpdatedOn = timestamp;

		measurements.push_back(measurement);
	}

	publisher.DefineMetadata({ device }, measurements, {}, 1);
}

// Publishes every signal continuously; the publisher only serializes the signals a subscriber actually
// subscribed to, so each subscription naturally receives just its own set.
static std::vector<MeasurementPtr> BuildMeasurements()
{
	const int64_t timestamp = ToTicks(UtcNow());
	std::vector<MeasurementPtr> measurements;

	for (int32_t ordinal = 1; ordinal <= TotalSignals; ordinal++)
	{
		const MeasurementPtr measurement = NewSharedPtr<Measurement>();
		measurement->SignalID = SignalID(ordinal);
		measurement->Value = SignalValue(ordinal);
		measurement->Adder = 0.0;
		measurement->Multiplier = 1.0;
		measurement->Timestamp = timestamp;
		measurements.push_back(measurement);
	}

	return measurements;
}

template<typename Predicate>
static void Wait(Predicate predicate, const std::string& message, const std::chrono::seconds timeout = 15s)
{
	const auto deadline = std::chrono::steady_clock::now() + timeout;

	while (!predicate())
	{
		Require(std::chrono::steady_clock::now() < deadline, "Timed out waiting for " + message);
		std::this_thread::sleep_for(5ms);
	}
}

// Waits for the subscription identified by expectedUpdates to deliver its complete signal set, then
// confirms a fresh sample contains nothing but that set.
static void ExpectSet(TestSubscriber& subscriber, const bool setB, const int32_t expectedUpdates, const std::string& phase)
{
	const std::set<int32_t> expected = SetOrdinals(setB);

	Wait([&] { return subscriber.Updates >= expectedUpdates; }, phase + " subscription update");

	// A publisher keeps using the previous cache until it processes the subscriber's cache confirmation,
	// so measurements belonging to the previous set may still legitimately arrive for a moment after the
	// update. Wait for every expected signal to show up, tolerating those stragglers.
	subscriber.Clear();

	Wait([&]
	{
		const std::set<int32_t> ordinals = subscriber.TakeOrdinals();
		return std::includes(ordinals.begin(), ordinals.end(), expected.begin(), expected.end());
	}, phase + " signals " + (setB ? "B" : "A"));

	// Sample again now that the transition is complete: nothing from the other set may remain.
	subscriber.Clear();
	std::this_thread::sleep_for(250ms);

	const std::set<int32_t> sampled = subscriber.TakeOrdinals();

	Require(!sampled.empty(), phase + " publication stopped");
	Require(sampled == expected, phase + " received signals outside the subscribed set");
}

static void Run(const bool compressed)
{
	TestPublisher publisher;
	DefineMetadata(publisher);

	// Bind an ephemeral port so concurrent runs never collide
	uint16_t port;
	{
		boost::asio::io_context service;
		boost::asio::ip::tcp::acceptor acceptor(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::loopback(), 0));
		port = acceptor.local_endpoint().port();
	}

	Require(publisher.Start("127.0.0.1", port), "Publisher failed to start");

	std::atomic<bool> publishing { true };

	std::thread publishThread([&]
	{
		while (publishing)
		{
			publisher.PublishMeasurements(BuildMeasurements());
			std::this_thread::sleep_for(20ms);
		}
	});

	auto subscriber = std::make_unique<TestSubscriber>();

	try
	{
		subscriber->SetAutoParseMetadata(false);
		subscriber->SetAutoReconnect(false);
		subscriber->SetPayloadDataCompressed(compressed);
		subscriber->SetFilterExpression(FilterExpression(false));
		subscriber->Initialize("127.0.0.1", port);
		subscriber->ConnectAsync();

		// First subscription of the connection - publisher cache index 0
		ExpectSet(*subscriber, false, 1, "initial");

		// Resubscribe with a different filter - publisher cache index 1, which is the case that
		// requires DataPacketFlags::CacheIndex to be set on every published data packet
		subscriber->SetFilterExpression(FilterExpression(true));
		ExpectSet(*subscriber, true, 2, "resubscribed");

		// Resubscribe once more - publisher cache index alternates back to 0
		subscriber->SetFilterExpression(FilterExpression(false));
		ExpectSet(*subscriber, false, 3, "realternated");

		subscriber->Disconnect();
	}
	catch (...)
	{
		publishing = false;
		publishThread.join();
		subscriber.reset();
		publisher.Stop();
		throw;
	}

	publishing = false;
	publishThread.join();
	subscriber.reset();
	publisher.Stop();

	std::cout << "PASS payload=" << (compressed ? "TSSC" : "compact") << std::endl;
}

int main()
{
	try
	{
		for (const bool compressed : { false, true })
			Run(compressed);

		std::cout << "All signal index cache alternation regressions passed." << std::endl;
		return 0;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "FAIL: " << ex.what() << std::endl;
		return 1;
	}
}
