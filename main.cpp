#include <hFramework.h>
#include <DistanceSensor.h>
#include <cmath>

using namespace hModules;

int maximum_distance = 100;
int target;
int shots = 0;
int power = 400;

//sensor
DistanceSensor sens(hSens1);

void hMain();

void launch(int distance_to_target)
{
    //angle
    hMotA.setEncoderPolarity(Polarity::Normal);  
    hMotA.setMotorPolarity(Polarity::Reversed);  

    //lock
    hMotB.setEncoderPolarity(Polarity::Normal);  
    hMotB.setMotorPolarity(Polarity::Reversed);  

    //reload
    hMotC.setEncoderPolarity(Polarity::Normal);  
    hMotC.setMotorPolarity(Polarity::Reversed);  

    Serial.printf("motors are connected\n");

    int angle = ceil((0.808 * distance_to_target) + 383);

    Serial.printf("angle is:%d ", angle);

    //angle set
    hMotA.rotRel(angle, 500, false, INFINITE);
    sys.delay(2000);

    //particle reload
    hMotB.rotRel(720, 300, true, INFINITE);
    sys.delay(2000);
    hMotB.rotRel(-720, 300, true, INFINITE);
    sys.delay(2000);
    hMotB.rotRel(-110, 300, true, INFINITE);
    sys.delay(2000);
    hMotB.rotRel(110, 300, true, INFINITE);
    sys.delay(2000);

    //launch
    hMotC.rotRel(180, 500, true, INFINITE);
    sys.delay(2000);
    hMotA.rotRel(-angle, 500, false, INFINITE);
    sys.delay(2000);
    hMotC.rotRel(-180, 500, true, INFINITE);
    sys.delay(5000);

    shots++;
}

void test_launch()
{
    //angle
    hMotA.setEncoderPolarity(Polarity::Normal);  
    hMotA.setMotorPolarity(Polarity::Reversed);  

    hMotA.rotRel(100, 500, true, INFINITE);
    hMotA.rotRel(-100, 500, true, INFINITE);

    shots++;
}

void target_detect(int target)
{
    int distance_to_target = target;
    int angle_sum = 0;

    while (1)
    {
        int distance = sens.getDistance();
        Serial.printf("distance : %d\r\n", distance);
        sys.delay(200);

        if (distance <= -1)
        {
            // continue;
            hMotD.rotRel(5, power, false, INFINITE);
            angle_sum += 5;
        }
        else if (distance > 0 && distance <= (target+5))
        {
            hMotD.rotRel(5, power, false, INFINITE);
            angle_sum += 5;
            Serial.printf("step\n");
        }
        else
        {
            break;
        }
    }

    Serial.printf("cycle stopped; calculating angle\n");

    int angle = ceil((angle_sum+20) / 2);
    // int dist = (target / tan((17*3.14159256)/180));
    // angle = angle + dist;
    Serial.printf("angle to rotate: %d", angle);

    hMotD.rotRel(-angle, power, false, INFINITE);
    sys.delay(1000);
    // launch(distance_to_target);
    test_launch();   
    sys.delay(1000);
    hMotD.rotRel(((angle)+30), power, false, INFINITE);
    sys.delay(3000);
    Serial.printf("starting new cycle\n");
}

void hMain()
{
    //platform motor
    hMotD.setEncoderPolarity(Polarity::Normal);  
    hMotD.setMotorPolarity(Polarity::Reversed);

    //sensor
    // DistanceSensor sens(hSens1);
    Serial.printf("motors and sensors are connected...\n");
    
    while (shots < 5)
    {
        int dist = sens.getDistance();
        
        Serial.printf("target detected at the distance: %d\r\n", dist);
        Serial.printf("number of shots: %d\r\n", shots);
        // Serial.printf("test1\n");

        sys.delay(200);

        if (dist <= maximum_distance)
        {
            if (dist <= -1)
            {
                // continue;
                hMotD.rotRel(10, power, false, INFINITE);
            }
            else
            {
                target = dist;
                // stop the platform
                Serial.printf("setting power to 0; target detected at distance: %d\r\n", dist);
                hMotD.setPower(0);
                sys.delay(3000);

                // launch
                target_detect(target);
                sys.delay(3000);
            }
        }

        else
        {
            hMotD.rotRel(10, power, false, INFINITE);
        }
    }  
    hMotD.setPower(0);
}