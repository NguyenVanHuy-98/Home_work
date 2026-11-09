# Hướng dẫn đọc code mô phỏng `ara::com`

Tài liệu này mô tả từng file, từng hàm và cách các file gọi nhau trong demo
`TemperatureSensor` (Provider - Consumer).

## 1. Tổng quan

Demo mô phỏng mô hình giao tiếp Service-Oriented của AUTOSAR Adaptive bằng C++ thuần:

- **Provider (Skeleton)**: đăng ký service, gửi event `TemperatureUpdate`, cung cấp method `GetCurrentTemperature`.
- **Consumer (Proxy)**: tìm service, subscribe event, nhận dữ liệu, gọi method.
- **Middleware giả lập**: `ara::com::Registry` đóng vai trò Service Discovery; `ara::com::Event<T>` đóng vai trò publish/subscribe.

Điểm quan trọng: trong AUTOSAR thật, `Skeleton`, `Proxy`, `Event`, `FindService`
được **sinh tự động từ ARXML**. Ở đây ta viết tay để hiểu bản chất.

## 2. Sơ đồ file

```text
Skeleton_&&_Proxy/
├── ara_com.hpp               # Thư viện ara::com giả lập
├── temperature_service.hpp   # Skeleton + Proxy của TemperatureSensor
├── temperature_provider.hpp  # Khai báo Provider
├── temperature_provider.cpp  # Cài đặt Provider
├── temperature_consumer.hpp  # Khai báo Consumer
├── temperature_consumer.cpp  # Cài đặt Consumer
├── log.hpp                   # Hàm log thread-safe
├── main.cpp                  # Khởi động Provider và Consumer
└── makefile                  # Build demo
```

## 3. Sơ đồ gọi hàm

```text
main()
 │
 ├── TemperatureConsumer(instance)
 │
 ├── std::thread ──► TemperatureConsumer::Run(7s)
 │                     │
 │                     ├── WaitForService()
 │                     │     └── TemperatureSensorProxy::FindService()
 │                     │           └── Registry::find()
 │                     │
 │                     ├── proxy_.emplace(handle)
 │                     │     └── TemperatureSensorProxy(handle)
 │                     │           └── lấy Payload từ ServiceInstance
 │                     │
 │                     ├── TemperatureUpdate.Subscribe(3)
 │                     ├── TemperatureUpdate.SetReceiveHandler(lambda)
 │                     │
 │                     └── lặp:
 │                           ├── proxy_->GetCurrentTemperature()
 │                           │     └── Payload::getCurrentTemperature()
 │                           │           └── TemperatureProvider::GetCurrentTemperature()
 │                           └── chờ 100 ms
 │
 ├── sleep(1.5s)
 │
 └── std::thread ──► TemperatureProvider::Start()
                       │
                       └── lặp mỗi 1 giây:
                             ├── ReadSensor()
                             ├── lastTemperature_.store()
                             └── TemperatureUpdate.Send()
                                   ├── đẩy sample vào State::samples
                                   └── gọi State::receiveHandler
                                         └── Consumer lambda
                                               └── GetNewSamples()
```

## 4. `ara_com.hpp` - thư viện middleware giả lập

File này chứa các thành phần cốt lõi mà về sau AUTOSAR sẽ sinh ra hoặc cung cấp sẵn.

### 4.1. `using InstanceSpecifier = std::string;`

Định danh service, ví dụ `/Vehicle/TemperatureSensor/1`.

Trong AUTOSAR thật, `InstanceSpecifier` là một kiểu riêng do `ara::core` cung cấp,
dùng để ánh xạ tới instance trong ARXML.

### 4.2. `template <typename T> class Event`

Mô phỏng event publish/subscribe của `ara::com`.

#### `struct State`

Trạng thái dùng chung giữa Skeleton và Proxy:

| Thành phần | Ý nghĩa |
|---|---|
| `std::mutex mutex` | Bảo vệ dữ liệu khi nhiều thread truy cập |
| `std::deque<T> samples` | Hàng đợi các sample chưa được đọc |
| `std::size_t maxSamples` | Số sample tối đa được giữ lại |
| `bool subscribed` | Consumer đã subscribe hay chưa |
| `receiveHandler` | Callback được gọi khi có sample mới |

#### `Event()`

Tạo một `State` mới. Dùng phía Skeleton.

#### `explicit Event(std::shared_ptr<State> state)`

Dùng chung `State` có sẵn. Dùng phía Proxy để nối vào event của Skeleton.

#### `void Send(const T& value)`

- Nếu chưa có subscriber thì bỏ qua.
- Thêm `value` vào `samples`.
- Nếu vượt `maxSamples` thì xoá sample cũ nhất.
- Gọi `receiveHandler` **sau khi đã mở khoá mutex** để tránh deadlock.

#### `void Subscribe(std::size_t maxSamples)`

Đánh dấu đã subscribe và đặt giới hạn hàng đợi.
Từ thời điểm này `Send` mới thực sự đưa dữ liệu cho Consumer.

#### `void SetReceiveHandler(std::function<void()> handler)`

Lưu callback. Callback được gọi mỗi khi `Send` có dữ liệu mới.
Trong AUTOSAR thật, callback này được middleware gọi trên một thread nội bộ.

#### `std::vector<T> GetNewSamples()`

Lấy toàn bộ sample đang có và xoá hàng đợi.
Đây là phiên bản đơn giản của `GetNewSamples` trong `ara::com` thật
(ở đó dùng `SamplePtr` và callback `receiver`).

#### `std::shared_ptr<State> state() const`

Trả về `State` để Skeleton chia sẻ cho Proxy.

### 4.3. `struct ServiceInstance`

Thông tin một service đang chạy:

| Thành phần | Ý nghĩa |
|---|---|
| `instance` | Định danh service |
| `payload` | Dữ liệu riêng của service, kiểu `shared_ptr<void>` |

`payload` giúp `Registry` không cần biết service cụ thể là gì.
Proxy sẽ `static_pointer_cast` về đúng kiểu `Payload` của mình.

### 4.4. `class ServiceHandle`

Đại diện cho một service mà Consumer tìm thấy.

#### `explicit ServiceHandle(std::shared_ptr<ServiceInstance>)`

Bọc `ServiceInstance`.

#### `const InstanceSpecifier& GetInstanceSpecifier() const`

Trả về tên instance.

#### `std::shared_ptr<ServiceInstance> instance() const`

Trả về `ServiceInstance` để Proxy lấy `payload`.

### 4.5. `class Registry`

Đóng vai trò Service Discovery nội bộ.

#### `static Registry& instance()`

Singleton, tạo Registry duy nhất cho toàn chương trình.

#### `void add(const InstanceSpecifier&, std::shared_ptr<ServiceInstance>)`

Provider gọi khi Skeleton được tạo.
Dùng mutex để bảo vệ `unordered_map`.

#### `void remove(const InstanceSpecifier&)`

Provider gọi khi Skeleton bị huỷ.
Service sẽ không còn được `FindService` tìm thấy.

#### `std::vector<std::shared_ptr<ServiceInstance>> find(const InstanceSpecifier&)`

Consumer gọi để tìm service.
Hiện tại trả về 0 hoặc 1 instance; AUTOSAR thật có thể trả về nhiều instance.

## 5. `log.hpp` - log an toàn giữa các thread

### `std::mutex& logMutex()`

Trả về một mutex dùng chung, tránh các dòng log bị xen kẽ khi Provider,
Consumer và Main cùng in ra.

### `template <typename... Args> void Log(const std::string& tag, Args&&... args)`

In `tag` và toàn bộ tham số trên một dòng, có khoá mutex.
Ví dụ:

```cpp
Log("[Provider]", "Send(TemperatureUpdate = ", temperature, " C)");
```

## 6. `temperature_service.hpp` - code "sinh ra" cho service

Đây là phần tương ứng với code mà công cụ AUTOSAR sinh ra từ file ARXML.

### 6.1. `struct TemperatureSensorService::Payload`

Dữ liệu thật mà Registry giữ:

| Thành phần | Ý nghĩa |
|---|---|
| `temperatureUpdate` | Trạng thái dùng chung của event `TemperatureUpdate` |
| `getCurrentTemperature` | Hàm gọi method `GetCurrentTemperature` |

`getCurrentTemperature` là `std::function<float()>`.
Nó cho phép Proxy gọi ngược vào Provider mà không cần biết lớp cụ thể.

### 6.2. `class TemperatureSensorSkeleton`

Lớp cơ sở cho Provider. Tương ứng `TemperatureSensorSkeleton` do AUTOSAR sinh ra.

#### `explicit TemperatureSensorSkeleton(ara::com::InstanceSpecifier instance)`

Thực hiện 5 việc:

1. Tạo event `TemperatureUpdate` với `State` mới.
2. Tạo `Payload`.
3. Gán `temperatureUpdate` vào Payload.
4. Gán lambda `[this]() { return GetCurrentTemperature(); }` vào Payload.
5. Tạo `ServiceInstance` và đăng ký vào `Registry`.

Sau bước này, Consumer có thể `FindService` thấy service.

#### `virtual ~TemperatureSensorSkeleton()`

Huỷ đăng ký service khỏi `Registry`.

#### `ara::com::Event<float> TemperatureUpdate`

Event công khai để Provider gọi `Send`.
Trong AUTOSAR thật, event cũng là thành viên public của skeleton.

#### `virtual float GetCurrentTemperature() = 0`

Method thuần ảo, buộc lớp con cài đặt.
Tương ứng operation `GetCurrentTemperature` trong ARXML.

### 6.3. `class TemperatureSensorProxy`

Lớp Consumer dùng để giao tiếp với service.

#### `explicit TemperatureSensorProxy(ara::com::ServiceHandle handle)`

1. Lưu `handle`.
2. Lấy `Payload` từ `handle.instance()->payload`.
3. Gán `TemperatureUpdate` dùng chung `State` với Skeleton.
4. Lưu hàm `getCurrentTemperature` để gọi method.

Sau bước này, Proxy và Skeleton chia sẻ cùng một event.

#### `static std::vector<ara::com::ServiceHandle> FindService(const ara::com::InstanceSpecifier&)`

Gọi `Registry::find` và bọc kết quả thành `ServiceHandle`.
Đây chính là `FindService` trong ví dụ AUTOSAR.

#### `float GetCurrentTemperature()`

Gọi `getCurrentTemperature_()`, thực chất chạy vào
`TemperatureProvider::GetCurrentTemperature()`.

#### `ara::com::Event<float> TemperatureUpdate`

Event phía Consumer. Consumer gọi `Subscribe`, `SetReceiveHandler`,
`GetNewSamples` trên đối tượng này.

## 7. `temperature_provider.hpp` / `.cpp` - Provider

### 7.1. `class TemperatureProvider : public TemperatureSensorSkeleton`

Lớp cài đặt cụ thể của service.

#### `explicit TemperatureProvider(ara::com::InstanceSpecifier instance)`

Gọi constructor của Skeleton để đăng ký service.
Khởi tạo `startTime_` để tính thời gian cho hàm `sin`.

#### `void Start()`

Vòng lặp chính của Provider:

1. Đọc nhiệt độ bằng `ReadSensor()`.
2. Lưu vào `lastTemperature_`.
3. Gửi event bằng `TemperatureUpdate.Send()`.
4. In log.
5. Ngủ 1 giây.
6. Lặp lại cho tới khi `running_ == false`.

Hàm này chạy trong thread riêng do `main` tạo.

#### `void Stop()`

Đặt `running_ = false`.
Thread `Start()` sẽ thoát sau tối đa 1 giây.

#### `float GetCurrentTemperature() override`

Trả về `lastTemperature_`.
Đây là dữ liệu mới nhất đã gửi qua event.

#### `float ReadSensor()`

Sinh nhiệt độ giả lập:

```text
25 + 3 * sin(thời_gian) + nhiễu_ngẫu_nhiên
```

Dùng `std::mt19937` và `std::normal_distribution` để tạo nhiễu nhỏ.

#### Các thành viên dữ liệu

| Thành viên | Ý nghĩa |
|---|---|
| `std::atomic<bool> running_` | Cờ dừng thread |
| `std::atomic<float> lastTemperature_` | Nhiệt độ mới nhất, đọc/ghi giữa hai thread |
| `startTime_` | Mốc thời gian bắt đầu |
| `rng_` | Bộ sinh số ngẫu nhiên |
| `noise_` | Phân phối nhiễu |

## 8. `temperature_consumer.hpp` / `.cpp` - Consumer

### 8.1. `class TemperatureConsumer`

#### `explicit TemperatureConsumer(ara::com::InstanceSpecifier instance)`

Lưu tên instance cần tìm.

#### `bool WaitForService(std::chrono::milliseconds timeout)`

1. Tính thời điểm hết hạn.
2. Gọi `TemperatureSensorProxy::FindService(instance_)`.
3. Nếu tìm thấy: tạo `TemperatureSensorProxy` trong `proxy_` và trả `true`.
4. Nếu chưa thấy: log, ngủ 250 ms, thử lại.
5. Hết thời gian: trả `false`.

Đây là mô phỏng Consumer khởi động trước Provider.

#### `void Run(std::chrono::seconds duration)`

Toàn bộ hoạt động của Consumer:

1. Gọi `WaitForService(5000 ms)`.
2. Nếu thất bại thì kết thúc.
3. `proxy_->TemperatureUpdate.Subscribe(3)`.
4. Đặt receive handler:
   - Gọi `GetNewSamples()`.
   - In từng nhiệt độ nhận được.
5. Trong `duration` giây:
   - Cứ mỗi 3 giây gọi `proxy_->GetCurrentTemperature()`.
   - Ngủ 100 ms giữa các lần kiểm tra.

Receive handler chạy trên thread của Provider (thread gọi `Send`),
không phải thread của Consumer. Đây là điểm quan trọng khi lập trình
với callback trong AUTOSAR.

#### `std::optional<TemperatureSensorProxy> proxy_`

Chỉ có giá trị sau khi tìm thấy service.
Dùng `std::optional` để biểu diễn trạng thái "chưa kết nối".

## 9. `main.cpp` - điều phối

### `int main()`

1. Khai báo instance `/Vehicle/TemperatureSensor/1`.
2. Tạo `TemperatureConsumer`.
3. Chạy `consumer.Run(7s)` trong thread riêng.
4. Ngủ 1.5 giây để Consumer thử `FindService` và thất bại vài lần.
5. Tạo `TemperatureProvider` (đăng ký service).
6. Chạy `provider.Start()` trong thread riêng.
7. Chờ Consumer chạy xong.
8. Gọi `provider.Stop()` và join thread Provider.
9. Kết thúc chương trình.

Thứ tự này mô phỏng tình huống thực tế: Consumer sống trước, Provider đến sau,
và Service Discovery phải xử lý việc đó.

## 10. Luồng dữ liệu chi tiết

### 10.1. Luồng đăng ký service

```text
main tạo TemperatureProvider
  -> TemperatureSensorSkeleton(...)
     -> Event<float>() tạo State mới
     -> tạo Payload
     -> Payload::getCurrentTemperature = lambda gọi virtual method
     -> Registry::add(instance, ServiceInstance)
```

### 10.2. Luồng tìm service và subscribe

```text
Consumer::WaitForService
  -> Proxy::FindService
     -> Registry::find
  -> Proxy(handle)
     -> lấy Payload
     -> Event<float>(Payload::temperatureUpdate)
  -> Event::Subscribe(3)
  -> Event::SetReceiveHandler(lambda)
```

### 10.3. Luồng event

```text
Provider::Start
  -> ReadSensor
  -> TemperatureUpdate.Send(value)
     -> State::samples.push_back(value)
     -> gọi receiveHandler
        -> Consumer lambda
           -> GetNewSamples()
           -> log
```

### 10.4. Luồng method

```text
Consumer::Run
  -> Proxy::GetCurrentTemperature()
     -> Payload::getCurrentTemperature()
        -> Skeleton's lambda
           -> TemperatureProvider::GetCurrentTemperature()
              -> lastTemperature_.load()
```

## 11. Đối chiếu với AUTOSAR Adaptive thật

| Trong demo | Trong AUTOSAR Adaptive |
|---|---|
| `ara_com.hpp` | Thư viện `ara::com` của hãng cung cấp |
| `Registry` | Service Discovery (SOME/IP-SD, UDP multicast) |
| `InstanceSpecifier` | Định danh instance từ ARXML |
| `TemperatureSensorSkeleton` | Code sinh từ `SERVICE-INTERFACE` trong ARXML |
| `TemperatureSensorProxy` | Code sinh từ `SERVICE-INTERFACE` trong ARXML |
| `Event<float>::Send` | Publish event qua SOME/IP hoặc IPC |
| `Subscribe` | Đăng ký nhận event |
| `SetReceiveHandler` | Middleware gọi callback khi có sample |
| `GetNewSamples` | Lấy sample từ middleware (thật: `SamplePtr`) |
| `GetCurrentTemperature` | Method request/response, thật: `ara::core::Future` |
| `std::thread` tự tạo | `ara::exec` quản lý process/thread |

## 12. Các thử nghiệm nên làm

1. **Bỏ `Subscribe`**: Consumer sẽ không nhận event vì `Send` kiểm tra `subscribed`.
2. **Đổi `Subscribe(3)` thành `Subscribe(1)`**: Chỉ sample mới nhất còn lại.
3. **Thêm Consumer thứ hai**: Cả hai cùng nhận event vì Skeleton hiện chỉ có một
   `State`; muốn nhiều subscriber độc lập cần mở rộng `Event` thành nhiều hàng đợi.
4. **Cho Provider dừng giữa chừng**: Consumer cần gọi lại `FindService` để phát hiện
   mất service.
5. **Thay `float` bằng struct**: Sửa `Event<float>` thành `Event<Temperature>` và
   cập nhật `Payload`, `Send`, `GetNewSamples`.
6. **Thêm method mới**: Thêm pure virtual vào Skeleton, thêm `std::function` vào
   `Payload`, thêm hàm gọi trong Proxy.

## 13. Lệnh build và chạy

```bash
cd 'Skeleton_&&_Proxy'
make          # build
make run      # build nếu cần rồi chạy
make clean    # xoá file thực thi
```

Hoặc build trực tiếp:

```bash
g++ -Wall -Wextra -std=c++17 -pthread -I. \
    main.cpp temperature_provider.cpp temperature_consumer.cpp \
    -o temperature_demo
./temperature_demo
```
