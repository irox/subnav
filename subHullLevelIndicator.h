// Copyright Ian Roxborough 2010

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoText2.h>

class SubHullLevelIndicator {

public:
//    SubHullPitchIndicator();

    SoSeparator* createSubmarinePitchLevelIndicator(float x, float y, float z);
    void updateSubmarinePitchLevelIndicator(float pitch);

private:
    SoRotationXYZ *pitchRotator;
    SoText2 *pitchText;
};

