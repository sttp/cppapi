//******************************************************************************************************
//  TransportTypes.h - Gbtc
//
//  Copyright � 2019, Grid Protection Alliance.  All Rights Reserved.
//
//  Licensed to the Grid Protection Alliance (GPA) under one or more contributor license agreements. See
//  the NOTICE file distributed with this work for additional information regarding copyright ownership.
//  The GPA licenses this file to you under the MIT License (MIT), the "License"; you may not use this
//  file except in compliance with the License. You may obtain a copy of the License at:
//
//      http://opensource.org/licenses/MIT
//
//  Unless agreed to in writing, the subject software distributed under the License is distributed on an
//  "AS-IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. Refer to the
//  License for the specific language governing permissions and limitations.
//
//  Code Modification History:
//  ----------------------------------------------------------------------------------------------------
//  03/09/2012 - Stephen C. Wills
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../CommonTypes.h"
#include "Constants.h"
#include <unordered_set>

namespace sttp::transport
{
    // Simple exception type thrown by the data subscriber
    class SubscriberException final : public Exception, public std::exception
    {
    private:
        std::string m_message;

    public:
        SubscriberException(std::string message) noexcept;
        const char* what() const noexcept override;
    };

    // Simple exception type thrown by the data publisher
    class PublisherException final : public Exception, public std::exception
    {
    private:
        std::string m_message;

    public:
        PublisherException(std::string message) noexcept;
        const char* what() const noexcept override;
    };

    // Fundamental POD type representing a measurement in STTP
    struct SimpleMeasurement
    {
        // Measurement's globally unique identifier.
        Guid SignalID { Empty::Guid };

        // Instantaneous value of the measurement.
        float64_t Value {};

        // The time, in ticks, that this measurement was taken.
        int64_t Timestamp {};

        // Flags indicating the state of the measurement as reported by the device that took it.
        MeasurementStateFlags Flags { MeasurementStateFlags::Normal };
    };

    // Extended data type representing a measurement in STTP
    struct Measurement
    {
        // Creates a new instance.
        Measurement();

        // Create instance from existing simple measurement
        explicit Measurement(const SimpleMeasurement& source);

        // Measurement's globally unique identifier.
        Guid SignalID { Empty::Guid };

        // Instantaneous value of the measurement.
        float64_t Value {};

        // Additive value modifier.
        float64_t Adder {};

        // Multiplicative value modifier.
        float64_t Multiplier { -1.0 };

        // The time, in ticks, that this measurement was taken.
        int64_t Timestamp {};

        // Flags indicating the state of the measurement as reported by the device that took it.
        MeasurementStateFlags Flags { MeasurementStateFlags::Normal };

        // Identification number used in human-readable measurement key.
        uint64_t ID {};

        // Source used in human-readable measurement key.
        std::string Source;

        // Human-readable tag name to help describe the measurement.
        std::string Tag;

        // Returns the value after applying the
        // multiplicative and additive value modifiers.
        [[nodiscard]] float64_t AdjustedValue() const;

        // Gets Timestamp as DateTime
        [[nodiscard]] datetime_t GetDateTime() const;

        // Gets Timestamp in Unix second of century and milliseconds
        void GetUnixTime(time_t& unixSOC, uint16_t& milliseconds) const;

        // Convert to POD type
        void GetSimpleMeasurement(SimpleMeasurement& measurement) const;
    };

    typedef SharedPtr<Measurement> MeasurementPtr;

    MeasurementPtr ToPtr(const Measurement& source);
    MeasurementPtr ToPtr(const SimpleMeasurement& source);

    enum class SignalKind : int16_t
    {
        Angle,            // Phase angle
        Magnitude,        // Phase magnitude
        Frequency,        // Line frequency
        DfDt,             // Frequency delta over time (dF/dt)
        Status,           // Status flags
        Digital,          // Digital value
        Analog,           // Analog value
        Calculation,      // Calculated value
        Statistic,        // Statistical value
        Alarm,            // Alarm value
        Quality,          // Quality flags
        Unknown           // Undetermined signal type
    };

    extern const char* SignalKindDescription[];

    extern const char* SignalKindAcronym[];

    // Map signal kind to signal type acronym
    std::string GetSignalTypeAcronym(SignalKind kind, char phasorType = 'I');

    // Map signal type acronym to engineering units
    std::string GetEngineeringUnits(const std::string& signalType);

    // Map protocol name to type
    std::string GetProtocolType(const std::string& protocolName);

    // Parses a measurement key in the format of "Source:ID" into its parts
    void ParseMeasurementKey(const std::string& key, std::string& source, uint64_t& id);

    // Helper function to parse signal kind
    SignalKind ParseSignalKind(const std::string& acronym);

    struct SignalReference
    {
        // Unique UUID of this individual measurement (key to MeasurementMetadata.SignalID)
        Guid SignalID { Empty::Guid };

        // Associated (parent) device for measurement (key to DeviceMetadata.Acronym / MeasurementMetadata.DeviceAcronym)
        std::string Acronym;

        // For phasors, digitals and analogs - this is the ordered index, uses 1-based indexing
        uint16_t Index {};

        // Signal classification (e.g., phase angle, but not specific type of voltage or current)
        SignalKind Kind { SignalKind::Unknown };

        SignalReference();
        explicit SignalReference(const std::string& signal);
    };

    std::ostream& operator << (std::ostream& stream, const SignalReference& reference);

    struct MeasurementMetadata
    {
        std::string DeviceAcronym;      // Associated (parent) device for measurement (key to DeviceMetadata.Acronym)
        std::string ID;                 // Measurement key string, format: "Source:ID" (if useful)
        Guid SignalID { Empty::Guid };	// Unique UUID of this individual measurement (lookup key!)
        std::string PointTag;           // Well formatted tag name for historians, e.g., OSI-PI, etc.
        SignalReference Reference;      // Parsed signal reference structure
        uint16_t PhasorSourceIndex {};  // Measurement phasor index, if measurement represents a "Phasor"
        std::string Description;        // Detailed measurement description (free-form)
        datetime_t UpdatedOn;           // Time of last meta-data update
    };

    typedef SharedPtr<MeasurementMetadata> MeasurementMetadataPtr;

    struct PhasorMetadata
    {
        std::string DeviceAcronym;      // Associated (parent) device for phasor (key to DeviceMetadata.Acronym)
        std::string Label;              // Channel name for "phasor" (covers two measurements)
        std::string Type;               // Phasor type, i.e., "V" for voltage or "I" for current
        std::string Phase;              // Phasor phase, one of, "+", "-", "0", "A", "B" or "C"
        uint16_t SourceIndex {};        // Phasor ordered index, uses 1-based indexing (key to MeasurementMetadata.PhasorSourceIndex)
        datetime_t UpdatedOn;           // Time of last meta-data update
    };

    typedef SharedPtr<PhasorMetadata> PhasorMetadataPtr;

    struct PhasorReference
    {
        PhasorMetadataPtr Phasor;           // Phasor metadata, includes phasor type, i.e., voltage or current
        MeasurementMetadataPtr Angle;       // Angle measurement metadata for phasor
        MeasurementMetadataPtr Magnitude;	// Magnitude measurement metadata for phasor
    };

    typedef SharedPtr<PhasorReference> PhasorReferencePtr;

    struct DeviceMetadata
    {
        std::string Acronym;			// Alpha-numeric device acronym, e.g., PMU/station name (all-caps - no spaces)
        std::string Name;               // User-defined device name / description (free-form)
        Guid UniqueID { Empty::Guid };  // Device unique UUID (used for C37.118 v3 config frame)
        uint16_t AccessID {};           // ID code used for device connection / reference
        std::string ParentAcronym;      // Original PDC name (if useful / not assigned for directly connected devices)
        std::string ProtocolName;       // Original protocol name (if useful)
        uint16_t FramesPerSecond {};    // Device reporting rate, e.g., 30 fps
        std::string CompanyAcronym;     // Original device company name (if useful)
        std::string VendorAcronym;      // Original device vendor name (if useful / provided)
        std::string VendorDeviceName;   // Original vendor device name, e.g., PMU brand (if useful / provided)
        float64_t Longitude {};         // Device longitude (if reported)
        float64_t Latitude {};          // Device latitude (if reported)
        datetime_t UpdatedOn;           // Time of last meta-data update

        // Associated measurement and phasor meta-data
        std::vector<MeasurementMetadataPtr> Measurements;   // DataPublisher does not need to assign
        std::vector<PhasorReferencePtr> Phasors;            // DataPublisher does not need to assign
    };

    typedef SharedPtr<DeviceMetadata> DeviceMetadataPtr;

    // Defines a synchrophasor-style configuration frame ordered structure for a device data frame
    struct ConfigurationFrame
    {
        std::string DeviceAcronym;
        MeasurementMetadataPtr QualityFlags; // This measurement may be null, see below **
        MeasurementMetadataPtr StatusFlags;
        MeasurementMetadataPtr Frequency;
        MeasurementMetadataPtr DfDt;
        std::vector<PhasorReferencePtr> Phasors;
        std::vector<MeasurementMetadataPtr> Analogs;
        std::vector<MeasurementMetadataPtr> Digitals;

        // Associated measurements
        std::unordered_set<Guid> Measurements;
    };

    typedef SharedPtr<ConfigurationFrame> ConfigurationFramePtr;

    // ** QualityFlags Note **
    //
    // The quality flags measurement contains the unsigned 32-bit integer value as
    // defined for the source protocol. In the case of IEEE C37.118, this will be the
    // TimeQualityFlags and TimeQualityIndicatorCode per the standard. To read the
    // value, look for the "Quality" signal kind and convert it to an uint32, e.g.:
    //
    //  for (auto &measurement : measurements)
    //  {
    //      const float64_t value = measurement->AdjustedValue();
    //      ConfigurationFramePtr configurationFrame;
    //      MeasurementMetadataPtr measurementMetadata;
    //  
    //      if (TryFindTargetConfigurationFrame(measurement->SignalID, configurationFrame))
    //      {
    //          if (TryGetMeasurementMetdataFromConfigurationFrame(measurement->SignalID, configurationFrame, measurementMetadata))
    //          {
    //              if (measurementMetadata->Reference.Kind == SignalKind::Quality)
    //              {
    //                  // Handle time quality flags
    //                  uint32_t timeQualityFlags = static_cast<uint32_t>(value);
    //              }
    //          }
    //      }
    //  }
    //
    // These time quality flags are only defined once per data frame and a data frame can
    // define multiple PMUs, e.g., in a data frame created by a PDC. When this C++ code
    // parses the incoming metadata, it defines only one PMU per configuration frame
    // structure. As a result, not all structures will have a quality flags defined, as
    // the source PMU may have come from a parent PDC data frame. The quality flags
    // measurement will only be defined when (1) the source was from a directly connected
    // PMU, i.e., a source data frame with exactly one PMU, and (2) the source protocol
    // supports a quality flags measurements, e.g., IEEE C37.118. Note that other source
    // protocols, e.g., IEEE 1344, do not define a quality flags value. When the quality
    // flags measurement is not available the QualityFlags measurement data pointer will
    // be null and consuming code should check for this expected condition.

    //struct Phasor
    //{
    //    Measurement Angle;
    //    Measurement Magnitude;
    //};
    
    // Holds the actual values, in order, for a device frame at a specific timestamp
    //struct DataFrame
    //{
    //    string DeviceAcronym;
    //    DateTime SOC;
    //    int milliseconds;
    //    Measurement StatusFlags;
    //    Measurement QualityFlags;
    //    Measurement Frequency;
    //    vector<Phasor> Phasors;
    //    vector<Measurement> Analogs;
    //    vector<Measurement> Digitals;
    //};
}
