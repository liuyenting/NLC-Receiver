#include "camera.hpp"

#include <vector>
#include <stdexcept>
#include <sstream>

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

void Camera::setParameter(Camera::Parameters parameter, ...) {
	switch(parameter) {
	case Camera::BusSpeed:
		break;
	case Camera::Resolution:
		break;
	case Camera::FrameRate:
		break;
	case Camera::Exposure:
		break;
	default:
		break;
	}
}

void Camera::startAcquisition() {
	err = dc1394_capture_setup(camHandle, dmaCounts, DC1394_CAPTURE_FLAGS_DEFAULT);
	if(err != DC1394_SUCCESS) {
		freeCamera();
		throw std::runtime_error("Failed to start the acquisition session");
	}
}

void Camera::grabFrame() {
	err = dc1394_capture_dequeue(camHandle, DC1394_CAPTURE_POLICY_WAIT, frameHandle);
	if(err != DC1394_SUCCESS) {
		stopAcquisition();
		freeCamera();
		throw std::runtime_error("Failed to grab a frame");
	}

	dc1394_capture_enqueue(camHandle, frameHandle);

	return img;
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

void Camera::freeObject() {
	dc1394_free(objHandle);
}

void Camera::freeCamera() {
	dc1394_camera_free(camHandle);
}
