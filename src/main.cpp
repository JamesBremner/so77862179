#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

#define GRAVITY 9.8

class cPhysics
{
public:
    void clear()
    {
        myHeight = 0;
    }
    void setHeight(double h)
    {
        myHeight = h;
    }
    double height() const
    {
        return myHeight;
    }
    std::pair<double, double> position(double t);

private:
    double myHeight;
};

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Simulation",
              {50, 50, 1000, 500}),
          lbHeight(wex::maker::make<wex::label>(fm)),
          ebHeight(wex::maker::make<wex::editbox>(fm)),
          bnRun(wex::maker::make<wex::button>(fm)),
          plSim(wex::maker::make<wex::panel>(fm)),
          myPanelSimHeight(300)
    {
        lbHeight.move(50, 50, 100, 30);
        lbHeight.text("Height ( m )");
        ebHeight.move(175, 50, 100, 30);
        ebHeight.text("");
        bnRun.move(50, 300, 100, 50);
        bnRun.text("RUN");
        plSim.move(350, 10, 100, myPanelSimHeight);

        // event handlers

        bnRun.events().click(
            [this]()
            {
                myPhysics.clear();
                myPhysics.setHeight(atof(ebHeight.text().c_str()));
                myScale = 300 / myPhysics.height();
                myTime = 0;
                PixelPosition();
                myTimer = new wex::timer(fm, 1000);
                plSim.update();
            });

        fm.events().timer(
            [this](int id)
            {
                myTime += 0.2; // run simulation at 1/5 realtime
                PixelPosition();
                plSim.update();
            });

        plSim.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                S.rectangle({myPosition.first, myPosition.second,
                             10, 10});
            });

        show();
        run();
    }

private:
    wex::label &lbHeight;
    wex::editbox &ebHeight;
    wex::button &bnRun;
    wex::panel &plSim;

    int myPanelSimHeight;
    double myScale;

    double myTime;
    wex::timer *myTimer;

    std::pair<int, int> myPosition;

    cPhysics myPhysics;

    void PixelPosition();
};

std::pair<double, double> cPhysics::position(double t)
{
    std::pair<double, double> ret = std::make_pair(0, myHeight);
    double d = 0.5 * GRAVITY * t * t;
    ret.second += d;
    return ret;
}

void cGUI::PixelPosition()
{
    // current position in meters
    auto positionMeters = myPhysics.position(myTime);

    fm.text("Simulation at " 
        + std::to_string( myTime)
        + " secs "
        + std::to_string( positionMeters.first)
        + ", "
        + std::to_string( positionMeters.second));

    // convert to pixels
    myPosition.first = (int)(myScale * positionMeters.first);
    myPosition.second = (int)(myScale * positionMeters.second) - myPanelSimHeight;

    // reached the ground?
    if (myPosition.second > myPanelSimHeight)
        myPosition.second = myPanelSimHeight-10;
    std::cout << myPosition.first << " " << myPosition.second << "\n";
}

main()
{
    cGUI theGUI;
    return 0;
}
