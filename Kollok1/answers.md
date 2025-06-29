ВАРИАНТ 1

# Нулевая группа вопросов

### 1. **Приведите Win API, необходимое для решения Лабораторной работы номер 3**

```

// Создание события

HANDLE hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

HANDLE hContinueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

// Создание потока

HANDLE hThread = CreateThread(NULL, 0, MarkerThread, threadParam, 0, NULL);

// Ожидание события

WaitForSingleObject(hStartEvent, INFINITE);

// Установка события

SetEvent(hContinueEvent);

// Критическая секция

CRITICAL_SECTION cs;

InitializeCriticalSection(&cs);

EnterCriticalSection(&cs);

// работа с общими данными

LeaveCriticalSection(&cs);
```

### 2. **Что такое поток в ОС Windows;**

Потоком в Windows называется объект ядра, которому операционная система выделяет процессорное время для выполнения приложения. Каждому потоку принадлежат следующие ресурсы:

- код исполняемой функции;
- набор регистров процессора;
- стек для работы приложения;
- стек для работы операционной системы;
- блок окружения, который содержит служебную информацию для работы потока.

Все эти ресурсы образуют контекст потока в Windows. В Windows различаются потоки двух типов:

- рабочие потоки (working threads);
- потоки интерфейса пользователя (user interface threads).

Рабочие потоки выполняют различные фоновые задачи в приложении. Потоки интерфейса пользователя

связаны с окнами и выполняют обработку сообщений, поступающих этим окнам.

Каждое приложение имеет, по крайней мере, один поток, который называется первичным (primary) или

главным (main) потоком. В консольных приложениях это поток, который исполняет функцию main. В

приложениях с графическим интерфейсом это поток, который исполняет функцию WinMain.

### 3. **Что такое Мьютекс;**

Для решения проблемы взаимного исключения между параллельными потоками, выполняющимися в контексте разных процессов, в операционных системах Windows используется объект ядра мьютекс. Слово мьютекс является переводом английского слова mutex, которое в свою очередь является сокращением от

выражения mutual exclusion, что на русском языке значит взаимное исключение. Мьютекс находится в сигнальном состоянии, если он не принадлежит ни одному потоку. В противном случае мьютекс находится в несигнальном состоянии. Одновременно мьютекс может принадлежать только одному потоку.

Создается мьютекс вызовом функции CreateMutex, которая имеет следующий прототип:
```

HANDLE CreateMutex(

LPSECURITY_ATTRIBUTES lpMutexAttributes, // атрибуты защиты

BOOL bInitialOwner, // начальный владелец мьютекса

LPCTSTR lpName // имя мьютекса

);
```
### 4. **Что такое Событие (объект синхронизации);**

Событием называется оповещение о некотором выполненном действии. В программировании события используются для оповещения одного потока о том, что другой поток выполнил некоторое действие. Сама же задача оповещения одного потока о некотором действии, которое совершил другой поток, называется задачей условной синхронизации или иногда задачей оповещения.

В операционных системах Windows события описываются объектами ядра Events. При этом различают

два типа событий:

- события с ручным сбросом;
- события с автоматическим сбросом.

Различие между этими типами событий заключается в том, что событие с ручным сбросом можно перевести в несигнальное состояние только посредством вызова функции ResetEvent, а событие с автоматическим сбросом переходит в несигнальное состояние как при помощи функции ResetEvent, так и при помощи функции

ожидания. При этом отметим, что если события с автоматическим сбросом ждут несколько потоков, используя функцию WaitForSingleObject, то из состояния ожидания освобождается только один из этих потоков.

Создаются события вызовом функции CreateEvent, которая имеет следующий прототип:
```

HANDLE CreateEvent(

LPSECURITY_ATTRIBUTES lpSecurityAttributes, // атрибуты защиты

BOOL bManualReset, // тип события

BOOL bInitialState, // начальное состояние события

LPCTSTR lpName // имя события

);
```
### 5. **Сравнительный анализ стандарта C++ 98 и любого более свежего стандарта (в контексте лабораторных)**
- Создание потока:
    - С++ 98:
       ```
       #include <windows.h>
        HANDLE hThread;
        DWORD threadId;
        hThread = CreateThread(
            nullptr,          // default security attributes
            0,                // default stack size
            MyThreadFunction, // thread function
            nullptr,          // parameter to thread function
            0,                // default creation flags
            &threadId         // returns the thread identifier
        );
       ```
    - C++ 11
      ```
      #include <thread>
      std::thread t(myThreadFunc);
      t.join(); // Ждём завершения потока
      ```
- Создание события:
    - С++ 98:
       ```
       WaitForSingleObject(hStartEvent, INFINITE);
       ```
    - C++ 11
      ```
      #include <condition_variable>
      std::condition_variable cv;
      bool eventHappened = false;
      ```
- Установка события:
    - С++ 98:
       ```
       SetEvent(hContinueEvent);
       ```
    - C++ 11
      ```
      eventHappened = true;
      cv.notify_one()
      ```
- Ожидание события:
    - С++ 98:
       ```
       WaitForSingleObject(hStartEvent, INFINITE);
       ```
    - C++ 11
      ```
      cv.wait(lock, [] { return eventHappened; });
      ```
- Работа с общими данными:
    - С++ 98:
       ```
       // Критическая секция
 
       CRITICAL_SECTION cs;
       
       InitializeCriticalSection(&cs);
       
       EnterCriticalSection(&cs);
       ```
    - C++ 11
      ```
      #include <mutex>
      std::mutex mtx;
      std::lock_guard<std::mutex> lock(mtx);
      ```

# Общие вопросы

### 1. **Что такое ООП? – полное определение;**

Объектно-ориентированное программирование (ООП) — это парадигма программирования, основанная на концепции объектов, которые объединяют данные (поля, атрибуты) и методы (функции) для работы с этими данными.

4 основных принципа ООП:

1. Инкапсуляция – объединение данных и кода, работающего с этими данными, в большинстве случае это сводится к тому, чтобы не давать доступа к важным данным напрямую. Вместо этого мы создаем ограниченный набор методов, с помощью которых можно работать с нашими данными. (например, private-поля в классе, Get и Set методы).
2. Наследование – создание новых классов на основе существующих с заимствованием всех существующих методов родительского класса и дополнительным функционалом (class Derived : public Base).
3. Полиморфизм – возможность объектов с одинаковым интерфейсом вести себя по-разному (виртуальные функции, перегрузка операторов).
4. Абстракция – выделение ключевых характеристик объекта, игнорируя детали реализации (интерфейсы, абстрактные классы).

### 2. **Магическое число 7 Миллера? – привести не менее 7 примеров из IT;**

«Маги́ческое число́ семь плюс-ми́нус два» («кошелёк Ми́ллера», «зако́н Ми́ллера») — закономерность, обнаруженная американским учёным-психологом Джорджем Миллером, согласно которой кратковременная человеческая память, как правило, не может запомнить и повторить более 7 ± 2 элементов. Джордж Миллер во время своей работы в компании Bell Laboratories провёл ряд экспериментов, целью которых было изучение параметров памяти операторов. В результате опытов он обнаружил, что кратковременная память человека способна запоминать в среднем девять двоичных цифр, восемь десятичных цифр, семь букв алфавита и пять односложных слов — то есть человек способен одновременно помнить 7 ± 2 элементов.

7 примеров из IT:

Пользовательский интерфейс (UI):
- Меню навигации: Ограничение количества пунктов в основном меню до 7 ± 2 позволяет пользователям легче ориентироваться и находить нужную информацию.
- Формы ввода: Разделение длинных форм на несколько шагов или блоков по 5-9 полей, чтобы избежать перегрузки пользователя.
- Отображение списков: Использование постраничной навигации или загрузки по мере прокрутки, чтобы не показывать слишком много элементов одновременно.
  Организация данных:
- Структура каталогов: Рекомендуется избегать слишком глубокой вложенности папок и стараться группировать файлы по 5-9 категориям.
- Базы данных: Оптимизация запросов и структуры таблиц для работы с небольшими наборами данных, что может повысить скорость обработки.
  Обучение и документация:
- Разбиение информации: Разделение больших объемов информации на небольшие блоки по 5-9 пунктов для более эффективного усвоения.
- Создание коротких инструкций: Формулировка понятных и лаконичных инструкций, не перегруженных деталями.
  Программирование:
- Функции и методы: Оптимизация длины функций и методов, чтобы они не были слишком громоздкими и сложными для понимания.
- Переменные и константы: Ограничение количества используемых переменных в определенном контексте для упрощения отладки и поддержки кода.

### 3. **Энтропия ПО? – привести не менее 5 примеров нег энтропийных мер при разработке ПО;**

Энтропия ссылается на второй закон термодинамики. Он гласит, что в закрытой изолированной системе количество хаоса со временем не уменьшается. Оно остаётся стабильным или растёт. Идея энтропии в ПО возникла благодаря книге Object-Oriented Software Engineering. Чем больше меняется программа, тем больше в ней хаоса, её энтропия увеличивается.

5 способов борьбы с энтропией:

1. Рефакторинг – устранение "кодового запаха" (например, разбиение god-object на мелкие классы).
2. Документация – комментарии, README, Swagger.
3. Юнит-тесты – автоматическая проверка корректности (например, тесты для CriticalSection в лабораторной).
4. CI/CD – автоматизация сборки и деплоя (GitHub Actions, GitLab CI).
5. Паттерны проектирования – Singleton для логгера, Factory для создания потоков.

###4. **5 признаков сложной системы по Гради Бучу – по каждому признаку привести не менее 2-х пояснительных примером из Ваших Лабораторных работ или pet проектов;**

Согласно Гради Бучу, сложная система обладает:

Иерархичностью

- Пример 1: В лабораторной с потоками есть иерархия: main → marker потоки.
- Пример 2: В pet-проекте с микросервисами – API-шлюз → сервисы → БД.

Наличием подсистем

- Пример 1: В ОС потоки → процессы → диспетчер памяти.
- Пример 2: В игре – рендеринг, физика, AI.

Слабой связанностью

- Пример 1: Потоки marker работают независимо, общаясь через Event.
- Пример 2: REST API с разделением на auth-service и data-service.

Эмерджентностью (свойства системы ≠ сумме свойств частей)

- Пример 1: Deadlock возникает только при взаимодействии потоков.
- Пример 2: Баг в многопоточной программе, которого нет в однопоточной версии.

Динамическим поведением

- Пример 1: Потоки marker могут блокироваться и возобновляться.
- Пример 2: Алгоритм балансировки нагрузки в кластере.

### 5. **Закон иерархических компенсаций Седова – привести не менее 5 исторических примеров применения закона Седова в эволюции IT сферы;**
“Закон Седова” в формулировке Назаретяна: В сложной иерархически организованной системе рост разнообразия на верхнем уровне обеспечивается ограничением разнообразия на предыдущих уровнях, и наоборот, рост разнообразия на нижнем уровне разрушает верхний уровень организации, то есть, система как таковая гибнет.
1. Переход от монолитов к микросервисам

   Монолит сложен → микросервисы просты, но их много.

2. Отказ от ASM в пользу языков высокого уровня

   Ассемблер сложен → Python прост, но требует интерпретатора.

3. Виртуализация (Docker, Kubernetes)

   Настройка серверов сложна → контейнеры просты, но оркестратор усложняется.

4. SQL → NoSQL

   Реляционные БД сложны для масштабирования → MongoDB проще, но теряет ACID.

5. REST → GraphQL

   REST требует множества эндпоинтов → GraphQL имеет один endpoint, но сложный парсер запросов.