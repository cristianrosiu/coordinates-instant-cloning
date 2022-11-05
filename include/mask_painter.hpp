#include "helpers.hpp"

#ifndef MASKPAINTER_H_
#define MASKPAINTER_H_

class MaskPainter
{
    std::string m_imagePath;
    cv::Mat m_image, m_mask, m_imageCopy, m_maskCopy;
    std::string m_windowName = "Mask Painter";
    bool m_mouseDown = false;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> points;

    static void paintMaskHandlerStatic(int event, int x, int y, int flags, void* that)
    {
        MaskPainter* thiz = static_cast<MaskPainter*>(that);
        thiz->paintMaskHandler(event, x, y);   
    }
    void paintMaskHandler(int event, int x, int const y);

    public:
        MaskPainter(std::string const &imagePath, std::string const &windowName);
        MaskPainter(std::string const &imagePath);
        void paintMask(std::string const &filename);



};

#endif