#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <dc1394/dc1394.h>
#include <vector>
#include <cstdarg>

namespace Device
{
class Camera;
}

class Camera
{
public:
	Camera();
	~Camera();

	std::vector<uint64_t> listDevices();
	void open(uint64_t guid);
	void close();

	enum Parameters {
		BusSpeed, Resolution, FrameRate, Exposure
	};

	void setParameter(Camera::Parameters parameter, ...);

	void startAcquisition();
	void grabFrame();
	void stopAcquisition();

private:
	void startTransmission();
	void stopTransmission();

	void freeObject();
	void freeCamera();

	dc1394_t *objHandle;
	dc1394camera_t *camHandle;
	dc1394error_t err;

	const uint_t dmaCounts = 4;
};

#endif
