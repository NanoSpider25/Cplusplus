#include "Shadow.h"

Shadow::Shadow(std::string filepath)
{
    //ctor
	shadowPiecewise = Equations(filepath);
}

Shadow::~Shadow()
{
    //dtor
}

vector<vector<double>> Shadow::Equations(string pointsPath) {
    /* get points, store in vector */
    std::ifstream inputFile(pointsPath);
    if (!inputFile) {
        std::cerr << "Error opening file." << std::endl;
    }

    std::vector<std::vector<int>> result;
    std::string line;

    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string firstPart, secondPart;
        std::getline(ss, firstPart, '#');
        std::getline(ss, secondPart, '#');
        int num1 = std::stoi(firstPart);
        int num2 = std::stoi(secondPart);
        result.push_back({ num1, num2 }); // {{1,2}, {3,4}, ...}
    }

    inputFile.close();
    vector<vector<vector<int>>> xy; // stores in like a cartisian plane, idea here is to go in a rectangle to join adjcent points
    // like for:
    // {-2,3} {-2,6}
    // {-3,3} {-3,4} blah blah blah
    // it gets stored like: {-2,3} {-2,6} {-3,3} {-3,4}
    // but we want it like: {-2,3} {-3,3} {-3,4} {-2,6}
    // so we can make an outline of the perimeter
    // this code does that:
    // also i accidentally stored y first then x, so the code might look sketchy but i was just lazy to change it
    // also its in the 4th quadrant cause the image was printing sideways for some reason
    // first we arrange the points in a 2d grid on a "cartesian plane"
    vector<vector<int>> x_y;
    //        ^- this is just storing a series of points, the vector<int> is always just size 2, y and x
    x_y.push_back(result[0]);// start with first val, starting point, then add vals around it to make a cart plane
    xy.push_back(x_y);
    for (const auto& pairy : result) {//loop through each point
        int sizey = xy.size();
        int i = xy.size() - 1;
        if (pairy[0] < xy[i][0][0]) { // check if the y val is above bigger one  {weoufh}
            vector<vector<int>> pairOf;//                                      ...
            pairOf.push_back(pairy);//                                         {add new layer}
            xy.push_back(pairOf); // add new y layer below last one            {bigger}
        }
        else if (xy[i][0][0] == pairy[0]) {// if y val is same, need to place in correct x val location
            bool inserted = false;// assume cant place anywhere i.e. its x val is bigger than all others
            for (int j = 0; j < xy[i].size(); j++) {
                if (pairy[1] < xy[i][j][1]) {// if we find a x val thats bigger, place bro before that
                    xy[i].insert(xy[i].begin() + j, pairy);
                    inserted = true;// tell us you done da deed
                    break;
                }
                if (pairy[1] == xy[i][j][1])// if bro already exists, bro already exists
                    inserted = true;
            }
            if (!inserted)// if not inserted, means he a big boi, add him to the end
                xy[i].push_back(pairy);
        }

    }
    //vector to store all points in the order we want
    vector<vector<int>> splinable;

    for (int i = 0; i < xy.size(); i++)
        splinable.push_back(xy[i][0]);// go down the first wall
    for (int i = 0; i < xy[xy.size() - 1].size(); i++)
        splinable.push_back(xy[xy.size() - 1][i]);// go right on the bottom floor
    for (int i = xy.size() - 1; i > -1; i--) {
        if (xy[i].size() > 1)
            splinable.push_back(xy[i][1]);// go up the end wall if there is an endwall
    }
    for (int i = xy[0].size() - 1; i > -1; i--)
        splinable.push_back(xy[0][i]);// go left on the top floor
    // all that stored in a line, so the direction is virtual
    vector<vector<int>> matricized;
    for (int i = 0; i < splinable.size() - 1; i++) {
        if (!(splinable[i][0] == splinable[i + 1][0] && splinable[i][1] == splinable[i + 1][1])) {
            matricized.push_back(splinable[i]);// add to new array if theres no consecutive dups somehow // basically dup removal
        }
    }
    matricized.push_back(splinable[0]);//add first point so we can loop all the way around
    splinable.clear();// save my laptops crying ram

    vector<vector<double>> matricizedD;
    for (const auto& row : matricized) {
        matricizedD.push_back(vector<double>(row.begin(), row.end()));// convert to double cause i forgot to do that in the first place
    }


    vector<vector<double>> eqs;//var to store the equations
    for (int i = 0; i < matricizedD.size() - 1; i++) {// loop through points, not the last one
        double x1 = matricizedD[i][1];
        double y1 = matricizedD[i][0];
        double x2 = matricizedD[i + 1][1];
        double y2 = matricizedD[i + 1][0];// get x and y for this point and the next

        double m = (y1 - y2) / (x1 - x2); //READ points and develop equation
        double c = y1 - m * x1;
        vector<double> vec;
        vec.push_back(m);
        vec.push_back(c);
        vec.push_back(min(x1, x2));
        vec.push_back(max(x1, x2));
        vec.push_back(min(y1, y2));
        vec.push_back(max(y1, y2));// store equation virtually as a vector to store the gradient, the c, and the bounds
        eqs.push_back(vec);// adds this eqution to the many equations of our virtual piecewise linear function
    }
    return eqs;
}
