#include <Adafruit_HMC5883_U.h>

class HeadingSensor {
  public:
    struct Heading {
      float degrees;
    };

    HeadingSensor();

    Heading getHeading();

  private:
    Adafruit_HMC5883_Unified mag;

    void applyDeclination(float &heading);
};
