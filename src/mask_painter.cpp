#include "mask_painter.hpp"

MaskPainter::MaskPainter(std::string const &imagePath, std::string const &windowName)
{
    m_imagePath = imagePath;
    m_windowName = windowName;

    m_image = cv::imread(imagePath);
    m_imageCopy = m_image.clone();

    m_mask = cv::Mat::zeros(m_image.size(), CV_64F);
    m_maskCopy = m_mask.clone();
}
MaskPainter::MaskPainter(std::string const &imagePath)
{
    m_imagePath = imagePath;

    m_image = cv::imread(imagePath);
    m_imageCopy = m_image.clone();

    m_mask = cv::Mat::zeros(m_image.size(), CV_64F);
    m_maskCopy = m_mask.clone();
}

void MaskPainter::paintMaskHandler(int event, int x, int y)
{
    switch (event)
    {
        case cv::EVENT_LBUTTONDOWN:
            m_mouseDown = true;
            contours.clear();
            points.clear();
            break;
        case cv::EVENT_MOUSEMOVE:
            if(!m_mouseDown) return;
            if (points.size() > 2) 
                cv::line(m_image, cv::Point(x,y),points[points.size()-1],cv::Scalar(0,255,0));
            points.push_back(cv::Point(x, y));
            cv::imshow(m_windowName, m_image);
            break;
        case cv::EVENT_LBUTTONUP:
            m_mouseDown = false;
            if(points.size() > 2)
            {
                contours.push_back(points);
                cv::drawContours(m_image, contours, 0, cv::Scalar(255), -1);
                cv::drawContours(m_mask, contours, 0, cv::Scalar(255), -1);
            }
            break;
    }
}
        
void MaskPainter::paintMask(std::string const &filename)
{
    cv::namedWindow(m_windowName);
    cv::setMouseCallback(m_windowName, paintMaskHandlerStatic, this);

    while(true)
    {
        cv::imshow(m_windowName, m_image);
        auto key = cv::waitKey(1) & 0xFF;

        if (key == 'r')
        {
            m_image = m_imageCopy.clone();
            m_mask = m_maskCopy.clone();
        }else if(key == 's')
        {
            break;
        }else if(key == 'q')
        {
            cv::destroyAllWindows();
            exit(0);
        }
    }
    
    auto ROI = m_mask;
    cv::imshow("Press any key to save the mask", ROI);
    cv::waitKey(0);
    cv::imwrite("data/masks/" + filename, m_mask);

    cv::destroyAllWindows();
}

// int main()
// {
//     int i = 0;
//     for (const auto &entry : std::filesystem::directory_iterator(dataDirPath.string() + "sources"))
//     {
//         auto painter = MaskPainter{entry.path()};
//         painter.paintMask(i);
//         i++;
//     }

//     return 0;
// }


