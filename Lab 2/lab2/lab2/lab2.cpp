#include <iostream>
#include "Model.h"
#include "Controller.h"
#include "View.h"

int main()
{
    try {
        auto model = std::make_shared<model::Model>("edbo", "postgres", "--");
        auto view = std::make_shared<view::View>();

        auto controller = std::make_shared<controller::Controller>(model, view);

        controller->interactionLoop();
    }
    catch (const std::exception& e){
        std::cout << "Exception occured: " << e.what() << std::endl;
    }
}
