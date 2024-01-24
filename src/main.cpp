#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

#define GRAVITY 9.8
#define DEG2RAD  0.01745

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
    void setAngle(double a)
    {
        myAngle = a;
    }
    double angle() const
    {
        return myAngle;
    }

    /// @brief Calculate position at specified time
    /// @param t
    /// @return
    std::pair<double, double> position(double t);

private:
    double myHeight; /// Start height
    double myAngle;
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
          lbAngle(wex::maker::make<wex::label>(fm)),
          ebAngle(wex::maker::make<wex::editbox>(fm)),
          bnRun(wex::maker::make<wex::button>(fm)),
          plSim(wex::maker::make<wex::panel>(fm)),
          myPanelSimHeight(300)
    {
        lbHeight.move(50, 50, 100, 30);
        lbHeight.text("Height ( m )");
        ebHeight.move(175, 50, 100, 30);
        ebHeight.text("");
        lbAngle.move(50, 100, 100, 30);
        lbAngle.text("Angle ( deg )");
        ebAngle.move(175, 100, 100, 30);
        ebAngle.text("");
        bnRun.move(50, 300, 100, 50);
        bnRun.text("RUN");
        plSim.move(350, 10, 300, myPanelSimHeight);

        // event handlers

        bnRun.events().click(
            [this]()
            {
                myPhysics.clear();
                myPhysics.setHeight(atof(ebHeight.text().c_str()));
                myPhysics.setAngle(atof(ebAngle.text().c_str()));
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
                draw(S);
            });

        show();
        run();
    }

private:
    wex::label &lbHeight;
    wex::editbox &ebHeight;
    wex::label &lbAngle;
    wex::editbox &ebAngle;
    wex::button &bnRun;
    wex::panel &plSim;

    int myPanelSimHeight;
    double myScale;

    double myTime;
    wex::timer *myTimer;

    std::pair<int, int> myPosition;

    cPhysics myPhysics;

    void PixelPosition();
    std::pair<int, int> PixelPosition(const std::pair<double, double> &positionMeters);
    void draw(wex::shapes &S);
};

std::pair<double, double> cPhysics::position(double t)
{
    std::pair<double, double> ret = std::make_pair(0, myHeight);
    if (myAngle < 1)
    {
        double d = 0.5 * GRAVITY * t * t;
        ret.second += d;
    } else {
        double d = 0.5 * sin(DEG2RAD* myAngle) * t * t;
        ret.first += d * sin(DEG2RAD* myAngle);
        ret.second += d * cos(DEG2RAD* myAngle);
    }
    return ret;
}

void cGUI::PixelPosition()
{
    // current position in meters
    auto positionMeters = myPhysics.position(myTime);

    fm.text("Simulation at " + std::to_string(myTime) + " secs " + std::to_string(positionMeters.first) + ", " + std::to_string(positionMeters.second));

    myPosition = PixelPosition(positionMeters);
    // std::cout << myPosition.first << " " << myPosition.second << "\n";
}

std::pair<int, int> cGUI::PixelPosition(const std::pair<double, double> &positionMeters)
{
    std::pair<int, int> ret;
    ret.first = (int)(myScale * positionMeters.first);
    ret.second = (int)(myScale * positionMeters.second) - myPanelSimHeight;

    // reached the ground?
    if (ret.second > myPanelSimHeight)
        ret.second = myPanelSimHeight - 10;
    return ret;
}

void cGUI::draw(wex::shapes &S)
{
    auto top = PixelPosition(std::make_pair(0.0, myPhysics.height()));
    auto bottom = PixelPosition(std::make_pair(myPhysics.height() / tan(DEG2RAD * myPhysics.angle()),300.0));
    S.line({top.first, top.second,
            bottom.first, bottom.second});
    S.rectangle({myPosition.first, myPosition.second,
                 10, 10});
}

main()
{
    cGUI theGUI;
    return 0;
}
