// Copyright (c) 2022. Johan Lind

#pragma once

#define NO_COPY_NO_MOVE(Class) \
  Class(const Class &) = delete;        \
  Class &operator=(const Class &) = delete; \
  Class(Class&& e) = delete; \
  Class& operator=(Class&& e) = delete;

#define NO_COPY(Class) \
  Class(const Class &) = delete;        \
  Class &operator=(const Class &) = delete;

#define NO_MOVE(Class) \
  Class(Class&& e) = delete; \
  Class& operator=(Class&& e) = delete;

