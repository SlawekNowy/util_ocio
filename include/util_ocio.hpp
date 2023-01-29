/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_OCIO_HPP__
#define __UTIL_OCIO_HPP__

#include <cinttypes>
#include <memory>
#include <string>
#include <optional>

namespace uimg {
	class ImageBuffer;
};
namespace util::ocio {
	class ColorProcessor : public std::enable_shared_from_this<ColorProcessor> {
	  public:
		struct CreateInfo {
			enum class BitDepth : uint8_t { Float32 = 0, Float16, UInt8 };
			std::string config;
			std::string configLocation;
			std::optional<std::string> lookName {};
			BitDepth bitDepth = BitDepth::Float32;
		};
		static std::shared_ptr<ColorProcessor> Create(const CreateInfo &createInfo, std::string &outErr, float exposure = 0.f, float gamma = 2.2f);
		bool Apply(uimg::ImageBuffer &imgBuf, std::string &outErr);
	  private:
		std::shared_ptr<void> m_processorData = nullptr;
	};
	bool apply_color_transform(uimg::ImageBuffer &imgBuf, const ColorProcessor::CreateInfo &createInfo, std::string &outErr, float exposure = 0.f, float gamma = 2.2f);
};

#endif
