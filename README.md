# Network simulator

See draft of the simulator code by [@rcomrad](https://github.com/rcomrad):

https://github.com/rcomrad/KusServer/tree/simulator/simulator

# Основные требования к функционалу и реализации симулятора
## 1. Цель
Разработать минимально функциональную систему симуляции передачи пакетов в сети. Система должна включать:
* Simulator и планировщик событий (EventScheduler)
* Базовый класс сетевого устройства NetworkDevice, от которого наследуются: Sender, Receiver, Switch.
* Каждый NetworkDevice имеет свою очередь пакетов и реализует базовые операции: принять пакет, отправить пакет, запланировать события передачи/приема.
## 2. Требуемые классы и функциональность
### Класс Event:
* Хранит время исполнения события и обработчик (функцию или std::function<void()>)
* Должен уметь возвращать время события (GetTime()) и вызывать обработчик (Execute()).
### Класс EventScheduler:
* Хранит события в приоритетной структурке данных, упорядоченной по времени.
* Методы:
* * Schedule(double time, std::function<void()>handler): добавить новое событие.
  * PopNextEvent(): извлечь и вернуть самое раннее событие.
  * PeekNextEventTime(): посмотреть время следующего события без извлечения.
  * Empty(): проверить, есть ли событие
### Класс Simulator:
* Хранит текущее симуляционное время, планировщик событий.
* Методы:
* * Run(): Запустить цикл обработки событий, пока они есть или пока не достигнут лимит времени.
  * StopAt(double time): задать время, после которого симуляция не должна продолжаться.
  * Schedule(double delay, std::function<void()> handler): запланировать событие относительно текущего времени.
  * Now(): вернуть текущее время симуляции.
### Класс NetworkDevice:
* Абстрактный или базовый класс для сетевых устройств.
* Хранит адрес (идентификтор), имеет очередь (буфер) для пакетов: std::queue<Packet>.
* Методы:
* * Enqueue(Packet p): положить пакет в очередь.
  * Dequeue(): Взять пакет из очереди.
  * Виртуальный метод ProcessPacket(Packet p), который в наследниках будет определять поведение при получении пакета.
* При поступлении пакета в очередь устройство должно запланировать событие, которое с учётом задержки и длительности обработки (или передачи) выведет пакет из очереди и отправит на следующий узел.
Время обработки: для упрощения можно считать, что устройство «обслуживает» по одному пакету за раз. Если устройство занято обработкой пакета, то следующий пакет ждёт в очереди. После обработки текущего пакета устройство планирует событие извлечения следующего.
Таким образом, длина очереди влияет на задержку: чем больше пакетов в очереди, тем дольше новый пакет будет ждать своей очереди на обработку.
###  Наследники NetworkDevice:
* Sender: получает пакеты от Flow. При генерации пакета Flow вызывает Sender::Enqueue(packet). Sender после обработки (по истечении очереди и задержки) отправляет пакет к следующему устройству (например, Switch).
* Switch: получает пакеты, кладёт их в очередь. Когда пакет оказывается «у головы» очереди и устройство готово его обработать, Switch через событие доставки отправляет пакет дальше (например, к Receiver).
* Receiver: получает пакеты, выводит сообщение о доставке и (опционально) генерирует ACK. ACK также идёт через очередь Receiver, прежде чем будет отправлен обратно (если логика симуляции это подразумевает).
### Очереди:
* Можно упростить и использовать std::queue<Packet>
* При получении пакета Switch вызывает Enqueue().
* Затем Simulator::schedule(...) для события ForwardPacket() через заданную задержку.
* При выполнении ForwardPacket() - Dequeue() и отправка пакета далее.
### Flow:
* Flow — источник пакетов. Он планирует регулярные события для генерации новых пакетов.
* Когда наступает время отправки нового пакета, Flow вызывает Sender::Enqueue(packet).
## 5. Учёт задержек и очередей:
* На каждом устройстве, после извлечения пакета из очереди, моделируется задержка обработки и передачи. Допустим, для Switch это: обработка занимает фиксированное время (например, 0.2 c) и лишь после этого планируется событие, которое отправляет пакет следующему устройству.
* Если очередь длинная, пакет будет ждать своей очереди на обработку. Следовательно, суммарная задержка = время ожидания в очереди + задержка обработки + задержка передачи к следующему узлу.
## 6. Планирования событий:
Задержка увеличивается, если очередь заполнена. Пакет, попавший в очередь длиной N, вынужден ждать завершения обработки предыдущих N пакетов.
Чтобы смоделировать это, при входе каждого пакета, если устройство свободно, планируется обработка «сразу». Если занято, обработка будет запланирована уже при завершении предыдущего пакета (через цепочку событий).
Таким образом, не нужно отдельно считать длину очереди во времени, она автоматически отразится в момент возникновения событий:
Пока события обработки не выполнены, новые пакеты накапливаются.
Чем больше накопилось пакетов, тем позже наступит событие обработки данного конкретного пакета.
