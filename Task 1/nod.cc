// Języki i narzędzia programowania
// Zadanie nr. 1
// Autorzy:
// Jakub Panasiuk, jp418362
// Bartosz Tużnik, bt418462
// Stanisław Świątkowski, ss421849
#include <iostream>
#include <string>
#include <regex>
#include <cmath>

// namespace which helps make the code cleaner
using namespace std;

// type which we use mainly do display hectometers but also
// to communicate errors to other parts of the programme
// and to signal that road has not been visited yet by a car
using flexHectometers = int;

// type which we use only to display hectometers, these
// ale kilometers multiplied by 10
using hectometers = uint_fast32_t;

// type which we only use to display road type which is either
// 'A' or 'S'
using roadType = char;

// type which we use to display road number, which is positive number
using roadNumber = uint_fast32_t;

// type which we use to display number of a line which we scanned
// either the car or error in
using lineNumber = uint_fast32_t;

// type used to display distance driven by a car, used by a single item
// in a programme and to make functions arguments more readable
using carDistanceDriven = map <string, pair<flexHectometers, flexHectometers>>;

// type used to display distance driven by cars on the road, used by a single item
// in a programme and to make functions arguments more readable
using routeDistanceDriven = map <pair<roadNumber , roadType>, hectometers>;

// type used to display cars' last presence on a road used by a single item
// in a programme and to make functions arguments more readable
using lastCarLine = map <string, pair <lineNumber, string>>;

// type used to display cars' current presence on a road used by a single item
// in a programme and to make functions arguments more readable
using carIsNowAt = map <string, pair <string, hectometers>>;

// type used to hold parsed line and information about it
using parsedLineInfo = tuple <string, string, flexHectometers>;

// enumerator which helps returning specific information about
// line that has been parsed:
// - blankLine: means parsed line is empty so
//              programme is skipping this line without an error given
// - question:  means that parsed line is a correctly given inquiry
// - error:     means that line is not correct and error will be printed
//              by parent functions
enum messages {blankLine = -3, question, error};

// returns true if string what is the license number consistent
// with the content of the task and false otherwise
bool isLicensePlate(string &what) {
    static const regex licensePlate("^[A-Za-z0-9]{3,11}$");
    return regex_match(what, licensePlate);
}

// returns true if string what is the name of road consistent
// with the content of the task and false otherwise
bool isRoadName(string &what) {
    static const regex roadName("^(A|S)([1-9][0-9]{0,2})$");
    return regex_match(what, roadName);
}

// returns true if string what is the number between 1 and 999
// and false otherwise
bool isCorrectNumber(string &what) {
    static const regex numberScheme("^(([1-9][0-9]*)|0),[0-9]$");
    return regex_match(what, numberScheme);
}

// given a string which is correct road name returns
// a pair of road number and road type
pair <roadNumber, roadType> changeRoadName(string &roadName) {
    roadType type = roadName[0];
    roadNumber roadNr = 0;
    for (auto it : roadName) {
        if (isdigit(it)) {
            roadNr *= 10;
            roadNr += it - '0';
        }
    }
    return {roadNr, type};
}

// converts and prints hectometers to kilometers, uses comma to separate
// integer part from fractional part
template <class T>
void convertHectometersToKilometers(T number) {
    T intPart = number / 10;
    T fractionalPart = number % 10;

    cout << intPart << ',' << fractionalPart;
}

// prints information about one car, it prints its license plate and sum of
// kilometers traveled on both highways and expressways,
// it only prints information about this type of route that the car has
// driven on
void printOneCar(string &licensePlate, flexHectometers highway,
                 flexHectometers expressway) {
    // checks if the car has been on some highway or expressway
    if (highway >= 0.0 || expressway >= 0.0) {
        cout << licensePlate;
        // checks if the car has driven on any highway
        if (highway >= 0.0) {
            cout << " A ";
            convertHectometersToKilometers<flexHectometers>(highway);
        }
        // checks if the car has driven on any expressway
        if (expressway >= 0.0) {
            cout << " S ";
            convertHectometersToKilometers<flexHectometers>(expressway);
        }
        cout << endl;
    }
}

// prints cars in lexicographic order
void printCars(carDistanceDriven &cars) {
    carDistanceDriven::iterator itr;

    for (itr = cars.begin(); itr != cars.end(); ++itr) {
        string licensePlate = itr->first;
        flexHectometers highway = itr->second.first;
        flexHectometers expressway = itr->second.second;

        printOneCar(licensePlate, highway, expressway);
    }
}

// prints routes sorted by their numbers (in case highway and expressway have
// the same number it prints highway first)
void printRoutes(routeDistanceDriven &routes) {
    routeDistanceDriven::iterator itr;

    for (itr = routes.begin(); itr != routes.end(); ++itr) {
        roadType currentRoadType = itr->first.second;
        roadNumber currentRoadNumber = itr->first.first;
        hectometers kilometrage = itr->second;

        cout << currentRoadType << currentRoadNumber << ' ';
        convertHectometersToKilometers<hectometers>(kilometrage);
        cout << endl;
    }
}

// prints license plate of a car  and the sum of kilometers traveled on both
// highways and expressways it prints the information only if the container for
// an element with a key equivalent to the given string is found
void printCarIfExists(carDistanceDriven &droveByCar, string &licensePlate) {
    carDistanceDriven::iterator it;
    it = droveByCar.find(licensePlate);

    if (it != droveByCar.end()) {
        pair <flexHectometers, flexHectometers> kilometrage =
                                                droveByCar.at(licensePlate);
        printOneCar(licensePlate, kilometrage.first, kilometrage.second);
    }
}

// prints name of the road and the sum of kilometers traveled on this road,
// it prints the information only if the container for an element with a key
// equivalent to the given string is found
void printRouteIfExists(routeDistanceDriven &droveOnRoute,
                        string &route) {
    routeDistanceDriven::iterator it;
    it = droveOnRoute.find(changeRoadName(route));

    if (it != droveOnRoute.end()) {
        hectometers kilometrage = droveOnRoute.at(changeRoadName(route));

        cout << route << " ";
        convertHectometersToKilometers(kilometrage);
        cout << endl;
    }
}

// processes inquiries beginning with the character "?", the licensePlateOrRoute
// stores optional argument for the query
void answerTo(carDistanceDriven &droveByCar,
              routeDistanceDriven &droveOnRoute,
              string &licensePlateOrRoute) {
    // inquiry without an argument, prints information about all cars and roads
    if (licensePlateOrRoute.empty()) {
        printCars(droveByCar);
        printRoutes(droveOnRoute);
    }
    // inquiry with an argument, prints car or road with a name matching the
    // given argument
    else {
        // checks if the given argument is a valid name for a license plate
        if (isLicensePlate(licensePlateOrRoute))
            printCarIfExists(droveByCar, licensePlateOrRoute);
        // checks if the given argument is a valid name for a road
        if (isRoadName(licensePlateOrRoute))
            printRouteIfExists(droveOnRoute, licensePlateOrRoute);
    }
}

void printErrorLine(lineNumber number, string &line) {
    cerr << "Error in line " << number << ": " << line << endl;
}

// returns true if road is highway, answer is based on its name
bool isItHighway(string &roadName) {
    return roadName[0] == 'A';
}

// divides string which has blank spaces into lesser ones
// divided by those blank spaced fe. " exam ple" -> "exam", "ple"
vector <string> divideLine(string &line) {
    static const regex noSpace("\\s+");
    vector <string> in;

    sregex_token_iterator begin(line.begin(), line.end(), noSpace ,-1), end;
    // line that divides that string and adds lesser ones onto a vector
    copy(begin, end, back_inserter(in));

    // this method has flaw that sometimes it adds empty string to a
    // vector so we search for it and remove them from vector
    for (size_t i = 0; i < in.size(); i++) {
        if (in[i].empty()) {
            in.erase(in.begin() + i);
            i--;
        }
    }

    return in;
}

// changes correctly given number string into a unsigned integer
hectometers changeToNum(string &numPretender) {
    hectometers integerPart = 0;

    for (auto it : numPretender) {
        if (it == ',')
            break;
        integerPart *= 10;
        integerPart += it - '0';
    }

    size_t length = numPretender.length();

    return integerPart * 10 + numPretender[length - 1] - '0';
}

// given a line of 3 strings returns third string changed to number if
// first two are correct respectively: license plate and road name
// otherwise is returns error code enum which is equal to -1
flexHectometers getDistOrError(vector <string> &in) {
    if (isLicensePlate(in[0]) && isRoadName(in[1]) && isCorrectNumber(in[2])) {
        return changeToNum(in[2]);
    }
    return error;
}

// parses a line which we know is a question line, may return error
// or a correct inquiry ready to be processed by other functions
parsedLineInfo parseQuestion(vector<string> &in) {
    // if after deleting question mark its a blank line
    if (in[0].empty() && in.size() == 1)
        return {"", "", question};

    // if this inquiry is like "?example"
    if (!in[0].empty() && in.size() == 1 &&
        (isLicensePlate(in[0]) || isRoadName(in[0])))
        return {in[0], "", question};
    // if this inquiry is like "? example"
    else if (in.size() == 2 && !in[1].empty() && in[0].empty() &&
             (isLicensePlate(in[1]) || isRoadName(in[1])))
        return {in[1], "", question};

    // there is no other options so it must be error
    return {"", "", error};
}

// parses a single line, may return error, command to add car road and a distance knot
// may also return a question line, which format is
// {asked_thing, "", question indication} where asked_thing may be
// blank, then the question is different
parsedLineInfo parseLine(string &line) {
    vector<string> in = divideLine(line);
    if (line.empty()) {
        return {"", "", blankLine};
    }
    // if there are 3 arguments it must be a command to add a car, because there
    // cant be inquiry with 3 separate arguments
    else if (in.size() == 3) {
        flexHectometers distanceKnot = getDistOrError(in);
        if (distanceKnot != -1)
            return {in[0], in[1], distanceKnot};
    }
    // we can assume that in[0][0] is a question mark iff
    // an inquiry is given on input, because every blank
    // character is deleted (even from the beginning), so always
    // first character of first word will be a question mark
    // when given correct query
    else if (in[0][0] == '?' && in.size() < 3) {
        in[0].erase(in[0].begin());
        return parseQuestion(in);
    }
    return {"", "", error};
}

// true value means that car was on the different road
// and it hasn't left it, false otherwise
bool hasCarTeleported(parsedLineInfo &parsedLine, carIsNowAt &currentCarPosition) {
    // first, check if the car isn't on any road and then
    // if it has not exited different road
    if (currentCarPosition[get<0>(parsedLine)].first.empty() ||
        currentCarPosition[get<0>(parsedLine)].first == get<1>(parsedLine))
        return false;

    return true;
}

// having unsigned integers its a function subtracting them
// because if we did it wrong unsigned integer would twist itself
// into wild number, we dont want that to happen
hectometers subtract(hectometers a, hectometers b) {
    if (a > b)
        return a - b;
    return b - a;
}

// updates car parameters on maps, if car is exiting a road it
// changes maps given in arguments to further make correct answers to
// given inquiries, if car had just entered a road it also registers it
// in appropriate way
void updateDistTravelled(string &carLicensePlate, string &roadName,
                         flexHectometers &distanceKnot,
                         carIsNowAt &currentCarPosition,
                         carDistanceDriven &droveByCar,
                         routeDistanceDriven &droveOnRoute) {
    // car is exiting the road
    if (!currentCarPosition[carLicensePlate].first.empty()) {
        if (droveByCar.find(carLicensePlate) == droveByCar.end()) {
            droveByCar[carLicensePlate].first = -1;
            droveByCar[carLicensePlate].second = -1;
        }
        uint_fast32_t distTravelled =
                subtract(distanceKnot, currentCarPosition[carLicensePlate].second);
        droveOnRoute[changeRoadName(roadName)] += distTravelled;
        currentCarPosition[carLicensePlate].first.clear();


        if (isItHighway(roadName)) {
            if (droveByCar[carLicensePlate].first < 0)
                droveByCar[carLicensePlate].first = distTravelled;
            else droveByCar[carLicensePlate].first += distTravelled;
        }
        else {
            if (droveByCar[carLicensePlate].second < 0)
                droveByCar[carLicensePlate].second = distTravelled;
            else droveByCar[carLicensePlate].second += distTravelled;
        }
    }
    // car is entering the road
    else
        currentCarPosition[carLicensePlate] = {roadName, distanceKnot};
}

// core of the programme, reads every line on input and
// adequately responds to it
void read() {
    string line;
    lineNumber inputLine = 0;

    // for every car we store number of kilometers drove by it on
    // highway (first parameter) and expressway (second parameter)
    carDistanceDriven droveByCar;
    // for every road stored which is presented by its
    // number (first argument) then its type (second argument) on
    // map we have number of kilometers drove on it by cars
    routeDistanceDriven droveOnRoute;
    // for every car we store its last presence on a road, which is
    // number of line when it was scanned and whole line
    lastCarLine tpCarError;
    // for every car we store its current presence on road, which is
    // presented by road on which it is on and distance knot
    carIsNowAt currentCarPosition;

    while (getline(cin, line)) {
        inputLine++;
        parsedLineInfo parsedLine = parseLine(line);
        flexHectometers distanceKnot = get <2> (parsedLine);

        if (distanceKnot >= 0) {
            string carLicensePlate = get <0> (parsedLine);
            string roadName = get <1> (parsedLine);

            if (hasCarTeleported(parsedLine, currentCarPosition)) {
                printErrorLine(tpCarError[carLicensePlate].first,
                               tpCarError[carLicensePlate].second);
                currentCarPosition[carLicensePlate] = {roadName, distanceKnot};
            }
            else {
                updateDistTravelled(carLicensePlate, roadName, distanceKnot,
                                    currentCarPosition, droveByCar, droveOnRoute);
            }

            tpCarError[carLicensePlate] = {inputLine, line};
        }
        else if (distanceKnot == question) {
            answerTo(droveByCar, droveOnRoute, get <0> (parsedLine));
        }
        else if (distanceKnot == error) {
            printErrorLine(inputLine, line);
        }
    }
}

int main() {
    read();
    return 0;
}