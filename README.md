# WTRC

Terminal tool to see weather forecasts from ilmeteo.net.

Actually I just wanted to write some C code, so I wrote this little client.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

To compile this program you need to have the development packages of glib-2.0, libcurl and libxml2.

For example, on Debian systems you can install them like this:

```
# apt-get install libglib2.0-dev libcurl4-openssl-dev libxml2-dev
```

### Configuring

The program contains just a few sample locations; you may add more of them by editing ```src/libweather_locations.h```.

You also need a valid token to use ilmeteo.net API. You must set your token in ```src/config.h```.

### Compiling

Once you've all the dependencies installed you may just compile the program by running make:

```
$ make
```

You can also generate the HTML documentation with Doxygen:
```
$ make doc
$ xdg-open doc/html/index.html
```

## Running the program

If wtrc compiled correctly, you will find the executable inside the ```src``` directory.

You can search for available locations:
```
$ src/wtrc --search='Acqua'
Location   : ACQUASPARTA (TR)
Coordinates: 42.691145, 12.546479
Code       : 28756

1 location found (5 locations available).
```

Then, you can get weather forecasts for a location:
```
$ src/wtrc -l Acquasparta
Weather forecasts for ACQUASPARTA (TR)

Date   Min (°) Max (°) Humidity (%) Wind(km/h) Weather
----   ------- ------- ------------ ---------- -------
Mon 12       7      13           91         19 Cloudy with moderate rain
Tue 13       6      13           79         16 Cloudy with thunderstorms
Wed 14       5      15           72          8 Cloudy
Thu 15       5      12           85         17 Cloudy with moderate rain
Fri 16       6      14           84         13 Cloudy with light rain
```

You can also get the forecasts by specifying the location code:
```
$ src/wtrc -l 28756
Weather forecasts for ACQUASPARTA (TR)

Date   Min (°) Max (°) Humidity (%) Wind(km/h) Weather
----   ------- ------- ------------ ---------- -------
Mon 12       7      13           91         19 Cloudy with moderate rain
Tue 13       6      13           79         16 Cloudy with thunderstorms
Wed 14       5      15           72          8 Cloudy
Thu 15       5      12           85         17 Cloudy with moderate rain
Fri 16       6      14           84         13 Cloudy with light rain
```

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
