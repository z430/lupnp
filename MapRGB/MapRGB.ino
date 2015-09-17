//MapRGB.ino
const int red = 8;
const int green = 10;
const int blue = 11;

int redVal;
int greenVal;
int blueVal;

int newRed = 0;
int newGreen = 0;
int newBlue = 0;

int lightness = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {

	redVal = 100;
	greenVal = 100;
	blueVal = 0;

	lightness+=5;
	if (lightness>255)
		lightness = 0;

	newRed = map(redVal, 0, 100, 0, lightness);
	newGreen = map(greenVal, 0, 100, 0, lightness);
	newBlue = map(blueVal, 0, 100, 0, lightness);

	analogWrite(red, newRed);
	analogWrite(green, newGreen);
	analogWrite(blue, newBlue);

	Serial.print(newRed);
	Serial.print("\t");
	Serial.print(newGreen);
	Serial.print("\t");
	Serial.println(newBlue);

	delay(100);
}
