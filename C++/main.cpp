

#include <iostream>
#include <string>
#include <optional>
#include <unordered_map>
#include <utility>

class Inventory{

    private:
    struct Product {
        std::string name ;
        double Price ;
        
    };
    std::unordered_map<int,Product> Product_;

    public: 
    void addProduct(int id, std::string name, float value ){
        Product_ [id] = Product{name,value} ;

    }

    std::optional<double> getPrice(int code) const{
        auto it = Product_.find(code) ;
        if (it == Product_.end() ) return std::nullopt ;
        return it->second.Price ; 
    }

    std::optional<std::pair<std::string, double>> getProduct(int code) const {

        auto it = Product_.find(code) ;
        if (it == Product_.end() ) return std::nullopt ;
        return std::make_pair(it->second.name , it->second.Price) ;

    }

    bool applyDiscount(int code ,double precent ){
        auto it = Product_.find(code) ;
        if ( it == Product_.end() ) return false ;

        it->second.Price *= (1.0 - precent / 100.0);
        return true;

    }

    std::optional< std::pair< std::string,double>> findCheapest() const {
        if(Product_.empty()) return std::nullopt  ;
        auto chepaset = Product_.begin() ;
        for (auto it = Product_.begin(); it != Product_.end() ; ++it ) {
            if (it->second.Price < chepaset->second.Price ) chepaset = it ;


        }
        
        return std::make_pair(chepaset->second.name , chepaset->second.Price) ;

    }
           
    double totalValue() const {
        double sum = 0.0;
        for (const auto& [code, p] : Product_) {
            sum += p.Price;
        }
        return sum;
    }

};





int main() {
    Inventory inv;
    inv.addProduct(1, "Laptop",  1500.0);
    inv.addProduct(2, "Mouse",     20.0);
    inv.addProduct(3, "Monitor",  300.0);

    if (auto p = inv.getPrice(1)) {
        std::cout << "Price: " << *p << '\n';
    }

    if (auto [name, price] = inv.getProduct(3).value_or(
            std::make_pair(std::string("N/A"), 0.0)); true) {

        std::cout << name << " = " << price << '\n';

    }

    auto p = inv.getProduct(999).value_or(
        std::make_pair(std::string("N/A"), 0.0));
    std::cout << p.first << " = " << p.second << '\n';

    bool ok = inv.applyDiscount(1, 10.0);
    std::cout << "Discount applied: " << std::boolalpha << ok << '\n';
    if (auto np = inv.getPrice(1)) {
        std::cout << "New price: " << *np << '\n';
    }

    std::cout << inv.applyDiscount(999, 10.0) << '\n';

    if (auto cheap = inv.findCheapest()) {
        auto [name, price] = *cheap;
        std::cout << "Cheapest: " << name << " (" << price << ")\n";
    }

    std::cout << "Total: " << inv.totalValue() << '\n';
}








//////////////////////////////////////////////////////////////////////////////////////////
// STD :: optional


// #include <iostream>
// #include <string>
// #include <optional>
// #include <unordered_map>
// #include <utility>

// class SensorRegistry {

// private:
//     struct Sensor {
//         std::string name;
//         float value;
//     };
//     std::unordered_map<int, Sensor> sensors_;

// public:
//     void addSensor(int id, const std::string& name, float value) {
//         sensors_[id] = Sensor{name, value};
//     }

//     std::optional<float> getValue(int id) const {
//         auto it = sensors_.find(id);
//         if (it == sensors_.end()) return std::nullopt;
//         return it->second.value;
//     }

//     std::optional<std::pair<std::string, float> > getSensor(int id) const {
//         auto it = sensors_.find(id);
//         if (it == sensors_.end()) return std::nullopt;
//         return std::make_pair(it->second.name, it->second.value);
//     }


// };

// int main() {

//     SensorRegistry reg ;
//     reg.addSensor(1, "temperature",12 ) ;
//     reg.addSensor(1, "temperature",12 ) ;


//     if (auto v = reg.getValue(1)){
//         std::cout << "value: " << *v << '\n'; 
//     }
//     if (auto [name, val] = reg.getSensor(1).value_or(
//         std::make_pair(std::string("N/A"), 0.0f)); true) {
//     std::cout << name << " = " << val << '\n';
//     }


// } ;





//////////////////////////////////////////////////////////////////////////////////////////
// Factory Pattern

// #include <iostream>
// #include <string>

// using namespace std;

// // Abstract class
// class Sensor{
//     public:
//         virtual void readData() = 0;
// };

// // Temperature class
// class TemperatureSensor : public Sensor{
//     public:
//         void readData() override {
//             cout<<"reading temp data: "<<endl;
//         }
// };

// // Humidity class
// class HumiditySensor : public Sensor{
//     public:
//         void readData() override {
//             cout<<"reading humidity data: "<<endl;
//         }
// };

// // Factory class (Creator)
// class SensorFactory{
//     public:
//         static Sensor* createSensor(const string& sensorType){
//             if(sensorType == "temp"){
//                 return new TemperatureSensor();     // trả về đối tượng TemperatureSensor
//             }
//             else if (sensorType == "humi"){
//                 return new HumiditySensor();        // trả về đối tượng PressureSensor
//             }
//             else{
//                 return nullptr;                     // trả về con trỏ null
//             }
//         }
// };
// int main(int argc, char const *argv[])
// {
//     Sensor* sensor = SensorFactory::createSensor("humi");
//     sensor->readData();
//     return 0;
// }

//////////////////////////////////////////////////////////////////////////////////////////
// Observer Pattern
// #include <iostream>
// #include <vector>
// #include <string>
// #include <algorithm>

// // Interface for observers (display, logger, etc.)
// class Observer {
// public:
//     virtual void update(float temperature, float humidity, float light) = 0;
// };

// // Subject (SensorManager) holds the state and notifies observers
// class SensorManager {
//     float temperature;
//     float humidity;
//     float light;
//     std::vector<Observer*> observers;
// public:
//     void registerObserver(Observer* observer) {
//         observers.push_back(observer);
//     }

//     void removeObserver(Observer* observer) {
//         observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
//     }

//     void notifyObservers() {
//         for (auto observer : observers) {
//             observer->update(temperature, humidity, light);
//         }
//     }

//     void setMeasurements(float temp, float hum, float lightLvl) {
//         temperature = temp;
//         humidity = hum;
//         light = lightLvl;
//         notifyObservers();
//     }
// };

// // Display component (an observer)
// class Display : public Observer {
// public:
//     void update(float temperature, float humidity, float light) override {
//         std::cout << "Display: Temperature: " << temperature
//                   << ", Humidity: " << humidity
//                   << ", Light: " << light << std::endl;
//     }
// };

// // Logger component (an observer)
// class Logger : public Observer {
// public:
//     void update(float temperature, float humidity, float light) override {
//         std::cout << "Logging data... Temp: " << temperature
//                   << ", Humidity: " << humidity
//                   << ", Light: " << light << std::endl;
//     }
// };

// int main() {
//     SensorManager sensorManager;

//     Display display;
//     Logger logger;

//     sensorManager.registerObserver(&display);
//     sensorManager.registerObserver(&logger);

//     sensorManager.setMeasurements(25.0, 60.0, 700.0); // Simulate sensor data update
//     sensorManager.setMeasurements(26.0, 65.0, 800.0); // Another sensor update

//     return 0;
// }

//////////////////////////////////////////////////////////////////////////////////////////
// #include<iostream>
// using  namespace std ;


// class  DoiTuong{
//     protected:
//         string ten ;
//         int id ;

//     public:
//         DoiTuong(){
//             static int ID = 1 ;
//             id  = ID ;
//             ID ++ ;
//         }

//         void SetName(string _ten) {
//             ten = _ten ;
//         }
//         virtual void display() {
//             cout << " ten: " << ten << endl ;
//             cout << " id: " << id << endl ;

//         }
    

// };
//  class SinhVien : public DoiTuong {
//     protected : 
//         string SinhVienChuyenNganh ;
//     public :
//         void SetChuyenNganh(string _ChuyenNganh){
//             SinhVienChuyenNganh = _ChuyenNganh ;
//         }

//         void display() {
//             cout << "ten: "<< ten << endl ;
//             cout << "id: "<< id << endl ;
//             cout << "chuyen nganh :" << SinhVienChuyenNganh << endl ;

//         }



//  };

//  class HocSinh : public DoiTuong{
//     protected:
//         string lop;
   
//     public:
//         void setLop(string _lop){
//             lop = _lop;
//         }

//         void display() override {
//             cout << "ten: " << ten << endl;
//             cout << "id: " << id << endl;
//             cout << "lop: " << lop << endl;
//         }
// };

//  class teacher : public DoiTuong {
//     protected : 
//         string gvChuyenMom ;
//     public :
//         void SetChuyenMom(string _ChuyenMom){
//             gvChuyenMom = _ChuyenMom ;
//         }

//     void display() {
//             cout << "ten: "<< ten << endl ;
//             cout << "id: "<< id << endl ;
//             cout << "chuyen mom :" << gvChuyenMom << endl ;
//     }


//  } ;

//  int main(int argc, char const *argv[]){

//     SinhVien sv1;
//     sv1.SetName("Trung");
//     sv1.SetChuyenNganh("TDH");

//     HocSinh hs1;
//     hs1.SetName("Tuan");
//     hs1.setLop("12A1");

//     teacher gv1;
//     gv1.SetName("Hoang");
//     gv1.SetChuyenMom("Toan");

//     DoiTuong *dt;

//     dt = &sv1;
//     dt->display();

//     dt = &hs1;
//     dt->display();

//     dt = &gv1;
//     dt->display();


//  }







//////////////////////////////////////////////////////////////////////////////////////////
// #include <iostream>
// using namespace std;

// class cha{
//     public:
//         virtual void display(){                            // Hàm ảo
//             cout << "display from class cha" << endl;
//         }
// };

// class con : public cha{
//     public:
//         void display() override{                           // Ghi đè hàm ảo
//             cout << "display from class con" << endl;
//         }
// };

// int main(){
//     cha *ptr;
//     con obj;

//     // trỏ con trỏ class cha đến đối tượng class con
//     ptr = &obj;

//     // Gọi hàm ảo
//     ptr->display();
// }