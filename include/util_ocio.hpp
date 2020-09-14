/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_OCIO_HPP__
#define __UTIL_OCIO_HPP__

#include <cinttypes>
#include <string>

namespace uimg {class ImageBuffer;};
namespace util::ocio
{
	enum class Config : uint8_t
	{
		FilmicBlender = 0
	};
	bool apply_color_transform(uimg::ImageBuffer &imgBuf,Config config,const std::string &configLocation,float exposure,float gamma,std::string &outErr);
};

#endif
