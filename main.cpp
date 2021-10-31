#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define CONTROL_WINDOW_NAME "Control"
#define VIDEO_WINDOW_NAME "Video"
#define ERROR_WINDOW_NAME "Error"

void openWindow(const cv::String& name) {
	cv::namedWindow(name);
	cvui::watch(name);
}

void closeWindow(const cv::String& name) {
	cv::destroyWindow(name);
	cv::waitKey(1);
}

int main(int argc, const char* argv[]) {
	cv::Mat control_frame = cv::Mat(220, 600, CV_8UC3);
	cv::Mat video_frame;
	cv::Mat error_frame = cv::Mat(100, 300, CV_8UC3);

	// Data
	cv::VideoCapture video;
	cv::String video_name = "10seconds.mp4";
	cv::String stream_name = "test.avi";
	int camera_index = 0;

	bool play_video = true;

	bool video_checked = true;
	bool camera_checked = false;

	int frame_width;
	int frame_height;
	cv::VideoWriter writer;
	int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
	double fps = 10.0;

	bool video_is_open = false;
	bool error = false;

	cvui::init(CONTROL_WINDOW_NAME, 20);

	while (true) {
		cvui::context(CONTROL_WINDOW_NAME);

		control_frame = cv::Scalar(100, 52, 49);

		cvui::beginColumn(control_frame, 50, 20, -1, -1, 10);
		cvui::checkbox("Camera: ", &camera_checked);
		cvui::checkbox("Video: ", &video_checked);

		if (cvui::button("Open")) {
			// video
			if (video_checked) {
				try {
					play_video = true;
					video.open(video_name);
					if (video.isOpened() == true) {
						openWindow(VIDEO_WINDOW_NAME);
						video_is_open = true;
					}
					else {
						throw(505);
					}
				}
				catch (...) {
					openWindow(ERROR_WINDOW_NAME);
					error = true;
				}
			}
			// camera
			else {
				try {
					video.open(camera_index);
					if (video.isOpened() == true) {
						frame_width = video.get(cv::CAP_PROP_FRAME_WIDTH);
						frame_height = video.get(cv::CAP_PROP_FRAME_HEIGHT);
						writer.open(stream_name, codec, fps, cv::Size(frame_width, frame_height), true);
						openWindow(VIDEO_WINDOW_NAME);
						video_is_open = true;
					}
					else {
						throw(505);
					}
				}
				catch (...) {
					openWindow(ERROR_WINDOW_NAME);
					error = true;
				}
			}
		}

		if (cvui::button("Play/Pause")) {
			play_video = !play_video;
		}

		if (cvui::button("Close")) {
			if (video_is_open) {
				video_is_open = false;
				closeWindow(VIDEO_WINDOW_NAME);
			}
			if (error) {
				error = false;
				closeWindow(ERROR_WINDOW_NAME);
			}
		}

		if (cvui::button("Quit")) {
			break;
		}
		cvui::endColumn();

		cvui::counter(control_frame, 200, 10, &camera_index);
		// Input does not work on columns or rows
		cvui::input(control_frame, 300, 10, 250, "stream_name", stream_name);
		cvui::input(control_frame, 200, 40, 350, "video_name", video_name);

		cvui::update(CONTROL_WINDOW_NAME);
		cv::imshow(CONTROL_WINDOW_NAME, control_frame);

		// Video
		if (video_is_open) {
			// video
			if (video_checked) {
				cvui::context(VIDEO_WINDOW_NAME);

				if (play_video) {
					video >> video_frame;
				}
				if (video_frame.empty()) {
					video_is_open = false;
					closeWindow(VIDEO_WINDOW_NAME);
				}
				else {
					cvui::update(VIDEO_WINDOW_NAME);
					cv::imshow(VIDEO_WINDOW_NAME, video_frame);
				}
			}
			// camera
			else {
				cvui::context(VIDEO_WINDOW_NAME);
				video >> video_frame;
				writer.write(video_frame);
				cv::imshow(VIDEO_WINDOW_NAME, video_frame);
			}
		}
		else {
			if (error) {
				cvui::context(ERROR_WINDOW_NAME);
				error_frame = cv::Scalar(10, 10, 100);
				cvui::text(error_frame, 15, 40, "Error opening video stream or file.", 0.5, 0xffffff);
				cvui::update(ERROR_WINDOW_NAME);
				cv::imshow(ERROR_WINDOW_NAME, error_frame);
			}
		}
	}

	return 0;
}
