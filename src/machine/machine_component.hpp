#ifndef MACHINE_COMPONENT_HPP
#define MACHINE_COMPONENT_HPP

class MachineComponent {
public:
  MachineComponent() {}
  virtual ~MachineComponent() {}

  virtual void clock() = 0;

  virtual void reset() = 0;

private:
};

#endif
