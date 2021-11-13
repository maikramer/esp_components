/*
 * FilteredInput.h
 *
 *  Created on: Jul 10, 2021
 *      Author: maikeu
 */

#ifndef INC_FILTEREDINPUT_H_
#define INC_FILTEREDINPUT_H_

#include <functional>
#ifdef STM32L1
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#elif defined(ESP_PLATFORM)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#endif

class FilteredInput {
public:
	FilteredInput(std::function<uint32_t()> readFunction, uint16_t debounce_ms) {
		_readFunction = readFunction;
		_debounceTime = pdMS_TO_TICKS(debounce_ms);
		_debounce = _debounceTime;
	}
	bool IsFiltered() {
		Update();
		return (_filtered);
	}
	uint32_t GetValue() {
		Update();
		return (_lastInput);
	}

	bool Changed() {
		Update();
		auto ret = _lastChangedCall != _lastInput;
		_lastChangedCall = _lastInput;
		return (ret);
	}

	void Update() {
		auto value = _readFunction();
		if (value == _lastInput) {
			_debounce -= xTaskGetTickCount() - _lastRead;
			_filtered = _debounce < 0;
		} else {
			_filtered = false;
			_debounce = _debounceTime;
		}
		_lastInput = value;
		_lastRead = xTaskGetTickCount();
	}
private:

	std::function<uint32_t()> _readFunction;
	bool _filtered = false;
	int32_t _debounce = 0;
	int32_t _debounceTime = 0;
	uint32_t _lastInput = 0;
	TickType_t _lastRead = 0;
	uint32_t _lastChangedCall = 0;
};

#endif /* INC_FILTEREDINPUT_H_ */
