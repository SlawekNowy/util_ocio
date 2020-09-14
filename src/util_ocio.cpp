/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <optional>
#include "util_ocio.hpp"
#include <util_image_buffer.hpp>
#include <sharedutils/util.h>
#include <sharedutils/util_path.hpp>

#include <OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/typedesc.h>
#include "OpenEXR/half.h"

bool util::ocio::apply_color_transform(uimg::ImageBuffer &imgBuf,Config configType,const std::string &configLocation,float exposure,float gamma,std::string &outErr)
{
	imgBuf.ToFloat();
	imgBuf.ApplyExposure(exposure);
	auto bitDepth = OCIO::BitDepth::BIT_DEPTH_F32;
	auto path = util::Path::CreatePath(util::get_program_path());
	auto context = OCIO::Context::Create();
	context->addSearchPath((path.GetString() +"modules/open_color_io").c_str());
	
	try
	{
		OCIO::ConstConfigRcPtr config = nullptr;
		OCIO::ConstProcessorRcPtr processor = nullptr;

		switch(configType)
		{
		case Config::FilmicBlender:
			config = OCIO::Config::CreateFromFile((configLocation +"filmic-blender/config.ocio").c_str());
			processor = config->getProcessor("scene_linear","Filmic sRGB");
			break;
		}
		if(processor == nullptr)
		{
			outErr = "Unable to retrieve processor for config type " +std::to_string(umath::to_integral(configType));
			return false;
		}

		auto cpuProcessor  = processor->getOptimizedCPUProcessor(bitDepth,bitDepth,OCIO::OPTIMIZATION_DEFAULT);

		OCIO::PackedImageDesc img{imgBuf.GetData(),static_cast<long>(imgBuf.GetWidth()),static_cast<long>(imgBuf.GetHeight()),imgBuf.GetChannelCount()};
		cpuProcessor->apply(img);

		imgBuf.ApplyGammaCorrection(gamma);
	}
	catch(OCIO::Exception & exception)
	{
		outErr = exception.what();
		return false;
	}
	catch(...)
	{
		outErr = "Unknown error";
		return false;
	}
	return true;
}
