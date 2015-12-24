#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <dc1394/dc1394.h>
#include <vector>
#include <cstdarg>
#include <opencv2/core/mat.hpp>

namespace Device
{
const uint32_t cameraDmaCounts = 4;
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
        BusSpeed = 0, Resolution, FrameRate, Exposure
	};
    void setParameter(int count, ...);

	void startAcquisition();
	cv::Mat grabFrame();
	void stopAcquisition();

private:
	void startTransmission();
	void stopTransmission();

	cv::Mat dc1394frameToMat(dc1394video_frame_t *frame);

	void freeObject();
	void freeCamera();

	dc1394_t *objHandle;
	dc1394camera_t *camHandle;
	dc1394video_frame_t *frameHandle;
	dc1394error_t err;
};

#endif
