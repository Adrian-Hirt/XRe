#pragma once

class Component {
public:
  virtual ~Component() = default;

  virtual void onAttach() = 0;
  virtual void onDetach() = 0;
};
