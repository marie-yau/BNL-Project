#include <iostream>
#include<fstream>
#include <sstream>
#include <vector>

using namespace std;

class Charge_Data {
    private:
        // store x,y,z coordinates of x-ray and x,y indexes of pixel of every point in quadrant 1
        vector<vector<int>> positions;
        // store charge fraction of every point in quadrant 1
        vector<double> charge_fractions;
        // convert pixel index to pixel coordinate
        int index_to_coordinate(int index);
        // convert pixel coordinate to pixel index
        int coordinate_to_index(int coordinate);
        /*
         * Takes x,z,y coordinates of the x-ray and x,y indexes of the pixel from quadrant 1 and returns the
         * corresponding charge fraction.
        */
        double get_quadrant_1_charge(int x_xray_coordinate, int y_xray_coordinate, int z_xray_coordinate,
                                     int x_pixel_index, int y_pixel_index);

    public:
        // constructor that reads quadrant 1 data from a csv file into `positions` and `charge_fractions` vectors
        Charge_Data(string file_path);
        /*
        * Takes x,z,y coordinates of the x-ray and x,y indexes of the pixel from any quadrant and returns the
        * corresponding charge fraction.
        */
        double get_charge(int x_xray_coordinate, int y_xray_coordinate, int z_xray_coordinate,
                          int x_pixel_index, int y_pixel_index);
};

int Charge_Data::index_to_coordinate(int index){
    switch(index){
        case 0: return -2;
        case 1: return -1;
        case 2: return 0;
        case 3: return 1;
        case 4: return 2;
    }
}

int Charge_Data::coordinate_to_index(int coordinate){
    switch(coordinate){
        case -2: return 0;
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
    }
}

double Charge_Data::get_quadrant_1_charge(int x_xray_coordinate, int y_xray_coordinate, int z_xray_coordinate,
                             int x_pixel_index, int y_pixel_index){
    for(int i = 0; i < charge_fractions.size(); i++){
        if(positions[i][0] == x_xray_coordinate && positions[i][1] == y_xray_coordinate &&
           positions[i][2] == z_xray_coordinate && positions[i][3] == x_pixel_index &&
           positions[i][4] == y_pixel_index){
            return charge_fractions[i];
        }
    }
    return -1;
}

Charge_Data::Charge_Data(string file_path){
    ifstream file(file_path);

    int x_xray_coordinate;
    int y_xray_coordinate;
    int z_xray_coordinate;
    int x_pixel_index;
    int y_pixel_index;
    double charge_fraction;

    if (file.is_open()) {
        string line;
        vector<int> position;
        getline(file,line);
        while (getline(file, line)) {
            istringstream iss(line);
            string value;

            getline(iss, value, ',');
            x_xray_coordinate = stoi(value);
            position.push_back(x_xray_coordinate);

            getline(iss, value, ',');
            y_xray_coordinate = stoi(value);
            position.push_back(y_xray_coordinate);

            getline(iss, value, ',');
            z_xray_coordinate = stoi(value);
            position.push_back(z_xray_coordinate);

            getline(iss, value, ',');
            x_pixel_index = stoi(value);
            position.push_back(x_pixel_index);

            getline(iss, value, ',');
            y_pixel_index = stoi(value);
            position.push_back(y_pixel_index);

            getline(iss, value, ',');
            charge_fraction = stod(value);

            charge_fractions.push_back(charge_fraction);
            positions.push_back(position);
            position.clear();
        }
        file.close();
    }
}

double Charge_Data::get_charge(int x_xray_coordinate, int y_xray_coordinate, int z_xray_coordinate,
                  int x_pixel_index, int y_pixel_index){
    double charge_fraction = -1;
    // quadrant 1
    if(x_xray_coordinate >= 0 && y_xray_coordinate >= 0){
        charge_fraction = get_quadrant_1_charge(x_xray_coordinate, y_xray_coordinate, z_xray_coordinate,
                                                x_pixel_index, y_pixel_index);
    }
    // quadrant 2
    else if(x_xray_coordinate < 0 && y_xray_coordinate >= 0){
        int original_x_xray_coordinate = y_xray_coordinate;
        int original_y_xray_coordinate = -x_xray_coordinate;
        int original_z_xray_coordinate = z_xray_coordinate;

        int x_pixel_coordinate = index_to_coordinate(x_pixel_index);
        int y_pixel_coordinate = index_to_coordinate(y_pixel_index);
        int original_x_pixel_coordinate = y_pixel_coordinate;
        int original_y_pixel_coordinate = -x_pixel_coordinate;
        int original_x_pixel_index = coordinate_to_index(original_x_pixel_coordinate);
        int original_y_pixel_index = coordinate_to_index(original_y_pixel_coordinate);

        charge_fraction = get_quadrant_1_charge(original_x_xray_coordinate, original_y_xray_coordinate,
                                                original_z_xray_coordinate,
                                                original_x_pixel_index, original_y_pixel_index);
    }
    // quadrant 3
    else if(x_xray_coordinate < 0 && y_xray_coordinate < 0){
        int original_x_xray_coordinate = -y_xray_coordinate;
        int original_y_xray_coordinate = -x_xray_coordinate;
        int original_z_xray_coordinate = z_xray_coordinate;

        int x_pixel_coordinate = index_to_coordinate(x_pixel_index);
        int y_pixel_coordinate = index_to_coordinate(y_pixel_index);
        int original_x_pixel_coordinate = -y_pixel_coordinate;
        int original_y_pixel_coordinate = -x_pixel_coordinate;
        int original_x_pixel_index = coordinate_to_index(original_x_pixel_coordinate);
        int original_y_pixel_index = coordinate_to_index(original_y_pixel_coordinate);

        charge_fraction = get_quadrant_1_charge(original_x_xray_coordinate, original_y_xray_coordinate,
                                                original_z_xray_coordinate,
                                                original_x_pixel_index, original_y_pixel_index);
    }
    // quadrant 4
    else{
        int original_x_xray_coordinate = -y_xray_coordinate;
        int original_y_xray_coordinate = x_xray_coordinate;
        int original_z_xray_coordinate = z_xray_coordinate;

        int x_pixel_coordinate = index_to_coordinate(x_pixel_index);
        int y_pixel_coordinate = index_to_coordinate(y_pixel_index);
        int original_x_pixel_coordinate = -y_pixel_coordinate;
        int original_y_pixel_coordinate = x_pixel_coordinate;
        int original_x_pixel_index = coordinate_to_index(original_x_pixel_coordinate);
        int original_y_pixel_index = coordinate_to_index(original_y_pixel_coordinate);

        charge_fraction = get_quadrant_1_charge(original_x_xray_coordinate, original_y_xray_coordinate,
                                                original_z_xray_coordinate,
                                                original_x_pixel_index, original_y_pixel_index);
    }
    return charge_fraction;

}

int main() {
    int x_xray_coordinate = -2;
    int y_xray_coordinate = 7;
    int z_xray_coordinate = 10;
    int x_pixel_index = 3;
    int y_pixel_index = 1;

    Charge_Data data = Charge_Data("quadrant_1.csv");
    double charge_fraction = data.get_charge(x_xray_coordinate, y_xray_coordinate, z_xray_coordinate,
                                             x_pixel_index, y_pixel_index);
    cout << charge_fraction;

    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            charge_fraction = data.get_charge(-4, 6, 10, i, j);
            cout << i << "," << j << ": " << charge_fraction << "\n";
        }
    }

    return 0;
}
