/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_OCIO_HPP__
#define __UTIL_OCIO_HPP__

#include <cinttypes>
#include <memory>
#include <string>

namespace uimg {class ImageBuffer;};
namespace util::ocio
{
	enum class Config : uint8_t
	{
		FilmicBlender = 0
	};
	class ColorProcessor
		: public std::enable_shared_from_this<ColorProcessor>
	{
	public:
		static std::shared_ptr<ColorProcessor> Create(Config config,const std::string &configLocation,std::string &outErr);
		bool Apply(uimg::ImageBuffer &imgBuf,std::string &outErr,float exposure=0.f,float gamma=2.2f);
	private:
		std::shared_ptr<const void> m_ocioContext = nullptr;
		std::shared_ptr<const void> m_ocioConfig = nullptr;
		std::shared_ptr<const void> m_ocioProcessor = nullptr;
	};
	bool apply_color_transform(uimg::ImageBuffer &imgBuf,Config config,const std::string &configLocation,std::string &outErr,float exposure=0.f,float gamma=2.2f);
};

#endif
