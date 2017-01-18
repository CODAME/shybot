#include <Adafruit_HMC5883_U.h>

class Heading {
  public:
     Heading(void(*FUNC_DEBUG) (String));

     float getHeadingDegrees();

  private:
    Adafruit_HMC5883_Unified mag;

    void applyDeclination(float &heading);
};
