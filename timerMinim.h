// мини-класс таймера, версия 2.0
// использован улучшенный алгоритм таймера на millis
// алгоритм чуть медленнее, но обеспечивает кратные интервалы и защиту от пропусков и переполнений
#pragma once
class TimerMinim
{
  public:
    TimerMinim(uint32_t interval);				// объявление таймера с указанием интервала
    TimerMinim() {}
    void setInterval(uint32_t interval);	// установка интервала работы таймера
    bool isReady();						// возвращает true, когда пришло время. Сбрасывается в false сам (AUTO) или вручную (MANUAL)
    void reset();							// ручной сброс таймера на установленный интервал

  private:
    uint32_t _timer = 0;
    uint32_t _interval = 0;
};
