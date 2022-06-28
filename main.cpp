#include <iostream>
#include<fstream>
#include <sstream>

using namespace std;

int index_to_coordinate(int index){
    switch(index){
        case 0: return -2;
        case 1: return -1;
        case 2: return 0;
        case 3: return 1;
        case 4: return 2;
    }
}

int coordinate_to_index(int coordinate){
    switch(coordinate){
        case -2: return 0;
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
    }
}

/*
 * This function takes x,z,y coordinates of the x-ray and x,y indexes of the pixel and returns the corresponding
 * charge fraction. The coordinates of the x-ray must be in the first quadrant (non-negative x and y coordinates' values).
 * @tparam: x,z,y coordinates of the x-ray and x,y indexes of the pixel
 * @returns: charge fraction corresponding to parameters passed to the function
 */
double get_original_charge(int original_x_xray_coordinate, int original_y_xray_coordinate, int original_z_xray_coordinate,
        int original_x_pixel_index, int original_y_pixel_index){
    // CHANGE 'quadrant_1.csv' FILE LOCATION HERE
    ifstream file("quadrant_1.csv");
    int x_xray_coordinate;
    int y_xray_coordinate;
    int z_xray_coordinate;
    int x_pixel_index;
    int y_pixel_index;
    double charge_fraction;

    if (file.is_open()) {
        string line;
        getline(file,line);
        while (getline(file, line)) {
            istringstream iss(line);
            string value;

            getline(iss, value, ',');
            x_xray_coordinate = stoi(value);
            if(x_xray_coordinate != original_x_xray_coordinate){
                continue;
            }

            getline(iss, value, ',');
            y_xray_coordinate = stoi(value);
            if(y_xray_coordinate != original_y_xray_coordinate){
                continue;
            }

            getline(iss, value, ',');
            z_xray_coordinate = stoi(value);
            if(z_xray_coordinate != original_z_xray_coordinate){
                continue;
            }

            getline(iss, value, ',');
            x_pixel_index = stoi(value);
            if(x_pixel_index != original_x_pixel_index){
                continue;
            }

            getline(iss, value, ',');
            y_pixel_index = stoi(value);
            if(y_pixel_index != original_y_pixel_index){
                continue;
            }

            getline(iss, value, ',');
            charge_fraction = stod(value);
            return charge_fraction;
        }
        file.close();
    }
    return -1;
}

/*
 * This function takes x,z,y coordinates of the x-ray and x,y indexes of the pixel and returns the corresponding
 * charge fraction.
 * @tparam: x,z,y coordinates of the x-ray and x,y indexes of the pixel
 * @returns: charge fraction corresponding to parameters passed to the function
 */
double get_charge(int x_xray_coordinate, int y_xray_coordinate, int z_xray_coordinate,
        int x_pixel_index, int y_pixel_index){
    double charge_fraction = -1;
    // quadrant 1
    if(x_xray_coordinate >= 0 && y_xray_coordinate >= 0){
        charge_fraction = get_original_charge(x_xray_coordinate, y_xray_coordinate,
                z_xray_coordinate, x_pixel_index, y_pixel_index);
        return 0;
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

        charge_fraction = get_original_charge(original_x_xray_coordinate, original_y_xray_coordinate, original_z_xray_coordinate,
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

        charge_fraction = get_original_charge(original_x_xray_coordinate, original_y_xray_coordinate, original_z_xray_coordinate,
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

        charge_fraction = get_original_charge(original_x_xray_coordinate, original_y_xray_coordinate, original_z_xray_coordinate,
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

    double charge_fraction;
    charge_fraction = get_charge(x_xray_coordinate, y_xray_coordinate, z_xray_coordinate, x_pixel_index, y_pixel_index);
    //q1: 7,2,10,1,1 -> 0.00666432
    //q2: -2,7,10,3,1 -> 0.00666432
    cout << charge_fraction;

    return 0;
}
