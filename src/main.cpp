#include "mvc_solver.hpp"
#include "mask_painter.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, const char* argv[])
{   
    bool noInput = false;
    std::string resultName;
    std::vector<int> offset {0, 0};
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("src,s", po::value<std::string>(), "source image path")
        ("trgt,t", po::value<std::string>(), "target image path")
        ("mask,m", po::value<std::string>(), "mask image path. if not specified a drawing window will appear")
        ("name,n", po::value<std::string>(&resultName)->default_value("output.png"), "name of output file")
        ("offset,o", po::value<std::vector<int>>(&offset), "Offset of patch")
        ("noInput,ni", po::bool_switch(&noInput), "uses inputs given in data folder (--i field required)")
        ("i,i", po::value<int>()->default_value(0), "number of inputs in data folder (--noInput field required)");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    
    po::notify(vm);

    if (noInput)
    {
        MVCSolver solver {};
        std::vector<glm::vec2> offset {glm::vec2{100, 20}, glm::vec2{180,200}, glm::vec2{90,175}, glm::vec2{148,150}, glm::vec2{115, 270}};
        for (int i = 0; i < 5 ;i++)
        {
           
            auto src = cv::imread(dataDirPath.string() + "/sources/" + "source_0" + std::to_string(i+1) + ".jpg");
            auto dest = cv::imread(dataDirPath.string() + "/targets/" + "target_0" + std::to_string(i+1) + ".jpg");
            auto mask = cv::imread(dataDirPath.string() + "/masks/" + "mask_0" + std::to_string(i+1) + ".png");
            
            auto test = solver.solve(src, dest, mask, offset[i]);
            cv::imwrite(outDirPath.string() + "/results/output_0" + std::to_string(i+1) + ".png", test);

            auto cropped = cv::Mat(test.size(), CV_8UC3, cv::Scalar(0,0,0));

            for (int y = 0; y < mask.rows; y++)
            {
                for (int x = 0; x < mask.cols; x++)
                {
                    if(mask.at<cv::Vec3b>(y, x)[0] != 255 ) continue;
                    cropped.at<cv::Vec3b>(y + offset[i].y, x+ offset[i].x) = test.at<cv::Vec3b>(y + offset[i].y, x+ offset[i].x);
                }
            }
            cv::imwrite(outDirPath.string() + "/results/output_cropped_0" + std::to_string(i+1) + ".png", cropped);

        }
        return 1;
    }
    
    if (vm.count("src") && vm.count("trgt")) {
        MVCSolver solver {};
        auto src = cv::imread(vm["src"].as<std::string>());
        auto dest = cv::imread(vm["trgt"].as<std::string>());

        cv::Mat result;
        if(vm.count("mask"))
        {
            auto mask = cv::imread(vm["mask"].as<std::string>());
            result = solver.solve(src, dest, mask, glm::vec2{offset[0], offset[1]});
        }else
        {
            MaskPainter painter {vm["src"].as<std::string>()};
            painter.paintMask("new_mask_rename.png");
            auto mask = cv::imread(dataDirPath.string() + "/masks/new_mask_rename.png", CV_8UC1);
            result = solver.solve(src, dest, mask, glm::vec2{offset[0], offset[1]});
        }
        cv::imwrite(outDirPath.string() + "/results/" + resultName, result);
        cv::imshow(resultName, result);
        cv::waitKey(0);
        std::cout << "Result saved to " + outDirPath.string() + "/results/" + resultName << "\n";
    }else{
        std::cout << "Please provide both the -s and the -t paths. Use --help,-h to check available commands\n";
        return 1; 
    }
 

    return 0;
}
