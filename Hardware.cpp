#include "Hardware.h"

bool Hardware::init = false;

void Hardware::initialize()
{
	if (init)
		return;
	
	if (!bcm2835_init())
		LOGE("FAILED TO INITALIZE HARDWARE");

	// Set all relay pins to output
	
    bcm2835_gpio_fsel(PUMPPIN, BCM2835_GPIO_FSEL_OUTP);	
	bcm2835_gpio_fsel(FEEDPIN, BCM2835_GPIO_FSEL_OUTP);
 	bcm2835_gpio_fsel(LIGHTPIN, BCM2835_GPIO_FSEL_OUTP);
 
	// Set sensor pins to input
	bcm2835_gpio_fsel(FEEDLEVELPIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(WATERLEVELPIN, BCM2835_GPIO_FSEL_INPT);
	
	// Turn everything off to be safe
	bcm2835_gpio_write(PUMPPIN, LOW);
	bcm2835_gpio_write(FEEDPIN, LOW);
	bcm2835_gpio_write(LIGHTPIN, LOW);
	init = true;
	LOGI("Hardware Init");
}

void Hardware::LightsOn()
{
	initialize();
	bcm2835_gpio_write(LIGHTPIN, HIGH);
	StateChange::GetInstance()->Change("LightOn");
}

void Hardware::LightsOff()
{
	initialize();
	bcm2835_gpio_write(LIGHTPIN, LOW);	
	StateChange::GetInstance()->Change("LightOff");
}

void Hardware::PumpsOn()
{
	initialize();
	bcm2835_gpio_write(PUMPPIN, HIGH);
	StateChange::GetInstance()->Change("PumpsOn");
}

void Hardware::PumpsOff()
{
	initialize();
	bcm2835_gpio_write(PUMPPIN, LOW);
	StateChange::GetInstance()->Change("PumpsOff");
}

void Hardware::FeedOn()
{
	initialize();
	bcm2835_gpio_write(FEEDPIN, HIGH);
	StateChange::GetInstance()->Change("FeedOn");
}

void Hardware::FeedOff()
{
	initialize();
	bcm2835_gpio_write(FEEDPIN, LOW);
	StateChange::GetInstance()->Change("LightOff");
}

bool Hardware::FeedLevel()
{
	initialize();
	uint8_t level = bcm2835_gpio_lev(FEEDLEVELPIN);
	if (level == HIGH)
		return true;
	return false;
}

bool Hardware::WaterLevel()
{
	initialize();
	uint8_t level = bcm2835_gpio_lev(WATERLEVELPIN);
	if (level == HIGH)
		return true;
	return false;
}