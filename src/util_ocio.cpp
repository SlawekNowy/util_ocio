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

#pragma optimize("",off)
std::shared_ptr<util::ocio::ColorProcessor> util::ocio::ColorProcessor::Create(const CreateInfo &createInfo,std::string &outErr)
{
	auto bitDepth = OCIO::BitDepth::BIT_DEPTH_F32;
	auto path = util::Path::CreatePath(util::get_program_path());
	auto context = OCIO::Context::Create();
	context->addSearchPath((path.GetString() +"modules/open_color_io").c_str());
	
	try
	{
		OCIO::ConstConfigRcPtr config = nullptr;
		OCIO::ConstProcessorRcPtr processor = nullptr;

		config = OCIO::Config::CreateFromFile((createInfo.configLocation +createInfo.config +"/config.ocio").c_str());
		if(createInfo.lookName.has_value())
		{
			auto look = config->getLook(createInfo.lookName->c_str());
			if(look)
				processor = config->getProcessor(look->getTransform());
		}
		if(processor == nullptr)
			processor = config->getProcessor("scene_linear","Filmic sRGB");
		if(processor == nullptr)
		{
			outErr = "Unable to retrieve processor for config type " +createInfo.config;
			return false;
		}

		auto cpuProcessor  = processor->getOptimizedCPUProcessor(bitDepth,bitDepth,OCIO::OPTIMIZATION_DEFAULT);
		auto oProcessor = std::shared_ptr<ColorProcessor>{new ColorProcessor{}};
		oProcessor->m_ocioContext = context;
		oProcessor->m_ocioConfig = config;
		oProcessor->m_ocioProcessor = std::static_pointer_cast<const OCIO::CPUProcessor>(cpuProcessor);
		return oProcessor;
	}
	catch(OCIO::Exception & exception)
	{
		outErr = exception.what();
		return nullptr;
	}
	catch(...)
	{
		outErr = "Unknown error";
		return nullptr;
	}
	// Unreachable
	return nullptr;
}
bool util::ocio::ColorProcessor::Apply(uimg::ImageBuffer &imgBuf,std::string &outErr,float exposure,float gamma)
{
	imgBuf.ToFloat();
	imgBuf.ApplyExposure(exposure);

	try
	{
		OCIO::PackedImageDesc img{imgBuf.GetData(),static_cast<long>(imgBuf.GetWidth()),static_cast<long>(imgBuf.GetHeight()),imgBuf.GetChannelCount()};
		static_cast<const OCIO::CPUProcessor*>(m_ocioProcessor.get())->apply(img);

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


bool util::ocio::apply_color_transform(uimg::ImageBuffer &imgBuf,const ColorProcessor::CreateInfo &createInfo,std::string &outErr,float exposure,float gamma)
{
	auto processor = ColorProcessor::Create(createInfo,outErr);
	if(processor == nullptr)
		return false;
	return processor->Apply(imgBuf,outErr,exposure,gamma);
}
#pragma optimize("",on)
