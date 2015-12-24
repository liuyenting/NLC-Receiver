#include "camera.hpp"

#include <vector>
#include <stdarg.h>
#include <stdexcept>
#include <sstream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>

Camera::Camera() {
	// Init the IIDC library.
	objHandle = dc1394_new();
	if(!objHandle)
		throw std::runtime_error("Failed to initialize libdc1394");
}

Camera::~Camera() {
	close();
}

std::vector<uint64_t> Camera::listDevices() {
	dc1394camera_list_t *list;
	err = dc1394_camera_enumerate(objHandle, &list);
	if(err != DC1394_SUCCESS)
		throw std::runtime_error("Failed to enumerate cameras");

	std::vector<uint64_t> deviceList;
	for(uint32_t i = 0; i < list->num; i++)
		deviceList.push_back(list->ids[i].guid);

	dc1394_camera_free_list(list);

	return deviceList;
}

void Camera::open(uint64_t guid) {
	camHandle = dc1394_camera_new(objHandle, guid);
	if(!camHandle) {
		std::ostringstream oss;
		oss << "Failed to initialize camera with GUID " << guid;
		throw std::runtime_error(oss.str());
	}
}

void Camera::close() {
	stopAcquisition();

	freeCamera();
	freeObject();
}

void Camera::setParameter(int count, ...) {
	va_list args;
	va_start(args, count);

	// Input is always in the form (Feature, Value).
	Camera::Parameters parameter;
	for(int i = 0; i < count; i++) {
        parameter = static_cast<Camera::Parameters>(va_arg(args, int));
		switch(parameter) {
		case Camera::BusSpeed:
		{
            dc1394speed_t busSpeed = static_cast<dc1394speed_t>(va_arg(args, int));
			err = dc1394_video_set_iso_speed(camHandle, busSpeed);
			if(err != DC1394_SUCCESS)
				throw std::runtime_error("Failed to switch bus speed");

			break;
		}

		case Camera::Resolution:
		{
			int left = va_arg(args, int);
			int top = va_arg(args, int);
			int width = va_arg(args, int);
			int height = va_arg(args, int);
			err = dc1394_format7_set_roi(camHandle,
			                             DC1394_VIDEO_MODE_FORMAT7_4,
			                             DC1394_COLOR_CODING_RGB8,
			                             DC1394_USE_MAX_AVAIL,
			                             left, top,
			                             width, height);
			if(err != DC1394_SUCCESS) {
				freeCamera();
				throw std::runtime_error("Failed to set format 7 configurations");
			}

			break;
		}

		case Camera::FrameRate:
		{
            dc1394framerate_t frameRate = static_cast<dc1394framerate_t>(va_arg(args, int));
			err = dc1394_video_set_framerate(camHandle, frameRate);
			if(err != DC1394_SUCCESS) {
				freeCamera();
				throw std::runtime_error("Failed to update frame rate");
			}

			break;
		}

		default:
			throw std::out_of_range("Unknown camera parameter");
		}
	}

	va_end(args);
}

void Camera::startAcquisition() {
	err = dc1394_capture_setup(camHandle, Device::cameraDmaCounts, DC1394_CAPTURE_FLAGS_DEFAULT);
	if(err != DC1394_SUCCESS) {
		freeCamera();
		throw std::runtime_error("Failed to start the acquisition session");
	}
}

cv::Mat Camera::grabFrame() {
	err = dc1394_capture_dequeue(camHandle, DC1394_CAPTURE_POLICY_WAIT, &frameHandle);
	if(err != DC1394_SUCCESS) {
		stopAcquisition();
		freeCamera();
		throw std::runtime_error("Failed to grab a frame");
	}

	cv::Mat newFrame = dc1394frameToMat(frameHandle);

	dc1394_capture_enqueue(camHandle, frameHandle);

	return newFrame;
}

void Camera::stopAcquisition() {
	err = dc1394_capture_stop(camHandle);
	if(err != DC1394_SUCCESS)
		throw std::runtime_error("Failed to stop the acquisition session");
	else
		stopTransmission();
}

void Camera::startTransmission() {
	err = dc1394_video_set_transmission(camHandle, DC1394_ON);
	if(err != DC1394_SUCCESS) {
		stopAcquisition();
		freeCamera();
		throw std::runtime_error("Failed to start isochronous transmission");
	}
}

void Camera::stopTransmission() {
	err = dc1394_video_set_transmission(camHandle, DC1394_OFF);
	if(err != DC1394_SUCCESS)
		throw std::runtime_error("Failed to stop the transmission");
}

cv::Mat Camera::dc1394frameToMat(dc1394video_frame_t *frame) {
	// Preset the Mat.
	cv::Size size(frame->size[0], frame->size[1]);
	cv::Mat tmp(size, CV_8UC3, frame->image);

	// Reform the RGB sequence.
	cv::Mat img(size, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::cvtColor(tmp, img, CV_RGB2BGR);
	tmp.release();

	return img;
}

void Camera::freeObject() {
	dc1394_free(objHandle);
}

void Camera::freeCamera() {
	dc1394_camera_free(camHandle);
}
