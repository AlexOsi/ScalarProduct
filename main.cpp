#include <iostream>
#include <thread>
#include <vector>
#include <exception>

class ThreadWrapper {
public:
    explicit ThreadWrapper(std::thread rawThread)
        : m_rawThread(std::move(rawThread)) {}
    ThreadWrapper(ThreadWrapper&& other) = default;
    ~ThreadWrapper() {
        join();
    }
    void join() {
        if (m_rawThread.joinable()) {
            m_rawThread.join();
        }
    }

private:
    std::thread m_rawThread;

};

void scalarProductPartial(const std::vector<int>& firstVector, const std::vector<int>& secondVector,
                  int left, int right, int indexOfPart, std::vector<int>& resultPartial) {
    if (firstVector.size() != secondVector.size()) {
        throw std::runtime_error("Vectors have different sizes");
    }
    for (int i = left; i < right; ++i) {
        resultPartial.at(indexOfPart) += firstVector.at(i)*secondVector.at(i);
    }


}

void fillVector(std::vector<int>& vector, int elementsNum) {
    vector.reserve(elementsNum);
    for (int i = 0; i < elementsNum; ++i) {
        vector.push_back(i);
    }
}

int main() {
    int elementsNum = 100;
    std::vector<int> firstVector;
    fillVector(firstVector, elementsNum);

    std::vector<int> secondVector;
    fillVector(secondVector, elementsNum);

    int numOfThreads = (std::thread::hardware_concurrency() > 0) ?
                       static_cast<int>(std::thread::hardware_concurrency()) : 2;

    std::vector<int> resultPartial(numOfThreads);


    int sizeOfPart = elementsNum / numOfThreads;
    int remainderElementsNum = elementsNum - sizeOfPart*numOfThreads;
    {
        std::vector<ThreadWrapper> threads;
        for (int i = 0; i < numOfThreads; ++i) {
            int left = i*sizeOfPart;
            int right = left + sizeOfPart;

            if (i == numOfThreads - 1) {
                right += remainderElementsNum;
                scalarProductPartial(firstVector, secondVector, left, right, i, resultPartial);
                break;
            }

            threads.emplace_back(std::thread(scalarProductPartial, std::ref(firstVector),
                                             std::ref(secondVector), left, right, i,
                                             std::ref(resultPartial)));

        }

    }
    int scalarProductOfVectors = 0;
    for (auto partial : resultPartial) {
        scalarProductOfVectors += partial;
    }
    std::cout << scalarProductOfVectors << std::endl;

/*    int result = 0;
    for (int i = 0; i < elementsNum; i++) {
        result += firstVector[i]*secondVector[i];
    }
    std::cout << result << std::endl;*/
    return 0;
}
