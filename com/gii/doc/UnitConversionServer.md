# UnitConversionServer Example {#sf-gii-UnitConversionServer}

## INI-File Format

This conversion server uses an ini-file format where the:
* **key**: `<unit>|<precision>`
* **value**: `<unit>,<multiplier>,<offset>,<precision>`

There need to be 2 section available. One for **Metric** and the other **Imperial** unit conversion. 

**Example file:**

```ini
[System Metric]
m/s|0=m/s,1,0,0
m/s|2=mm/s,1000,0,-1
m/s|3=mm/s,1000,0,0
m/s|4=mm/s,1000,0,1
m/s|5=mm/s,1000,0,2

rad|0=deg,57.2958,0,0
rad|1=deg,57.2958,0,0
rad|2=deg,57.2958,0,1
rad|3=deg,57.2958,0,2
rad|4=deg,57.2958,0,3
rad|5=deg,57.2958,0,4
rad|6=deg,57.2958,0,5

[System Imperial]
m/s|0="/s,39.37007874015748031496062992126,0,0
m/s|1="/s,39.37007874015748031496062992126,0,1
m/s|2="/s,39.37007874015748031496062992126,0,2
m/s|3="/s,39.37007874015748031496062992126,0,3
m/s|4="/s,39.37007874015748031496062992126,0,4
m/s|5="/s,39.37007874015748031496062992126,0,5

rad|0=deg,57.2958,0,0
rad|1=deg,57.2958,0,0
rad|2=deg,57.2958,0,1
rad|3=deg,57.2958,0,2
rad|4=deg,57.2958,0,3
rad|5=deg,57.2958,0,4
rad|6=deg,57.2958,0,5

°C|1=°F,1.8,32,1
```

## Usage


### Conversion Server

The instance of `::sf::UnitConversionServer` is created some where in the application. 

```c++
// Create input string stream with ini formatted data.
std::istringstream is(R"(

[System Metric]
m/s|1=mm/s,1000,0,-2
m/s|2=mm/s,1000,0,-1
m/s|3=mm/s,1000,0,0

[System Imperial]
m/s|0="/s,39.37007874015748031496062992126,0,0
m/s|1="/s,39.37007874015748031496062992126,0,1
m/s|2="/s,39.37007874015748031496062992126,0,2
°C|1=°F,1.8,32,1

)");

// Create and thus install unit conversion handler. 
sf::UnitConversionServer ucs(is);

// Use imperial units when converting.
ucs.setUnitSystem(sf::UnitConversionServer::usImperial);
```

### Conversion Client

Calling global function `#sf::getUnitConversion()` is now handled by the singleton `::sf::UnitConversionServer` instance.   
```c++
// Variables to fill with conversion information.
double multiplier{0}, offset{0}, value{32.2};
int precision{0};
std::string unit;

// Using UTF-8 encoded source.
sf::getUnitConversion("", "°C", 1, multiplier, offset, unit, precision);

// Using ASCII only encoded source.
sf::getUnitConversion("", "\U000000B0C", 1, multiplier, offset, unit, precision);

// Print the converted value.
std::clog << stringf("%.*lf", multiplier * value + offset, precision) << std::endl;
```


