// #define I2C_UNIT_TEST  // this macro will disable ioctl write function (write only)

#include <bus_adapter/i2c_adapter/i2c_adapter.h>
#include <device/device_info.h>
#include <device/tca/tca.h>

#include <chrono>

using ::lra::bus::I2c;
using ::lra::bus_adapter::i2c::I2cAdapter_S;
using ::lra::device::I2cDeviceInfo;
using ::lra::device::Tca9548a;

int main() {
  // Deviceinfo
  I2cDeviceInfo info;
  info.addr_ = 0x70;
  info.flags_ = 0;
  info.iaddr_bytes_ = 1;  // should be 0
  info.page_bytes_ = 16;  // ?
  info.tenbit_ = false;

  // bus
  I2c i2c;
  i2c.Init("/dev/i2c-1");
  printf("%d\n", i2c.speed_);

  // I2cAdapter init struct
  I2cAdapter_S init_s;
  init_s.bus_ = std::make_shared<I2c>(i2c);
  init_s.delay_ = 50;
  init_s.method_ = I2c::I2cMethod::kPlain;
  init_s.name_ = "tca";

  Tca9548a tca(info);
  tca.Init(init_s);

  // auto start = std::chrono::high_resolution_clock::now();
  while (true) tca.Write(tca.CONTROL, 0x1);  // write ok
  // auto end = std::chrono::high_resolution_clock::now();
  // printf("%ld\n", (end-start).count());
}