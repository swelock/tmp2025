#include "requesthandler.h"
#include "../../common/crypto/tripledes.h"
#include "../../common/math/chordmethod.h"
#include "../../common/steganography/steganography.h"
#include <QBuffer>
#include <QDebug>

RequestHandler::RequestHandler(QObject *parent) : QObject(parent) {
    m_authHandler = new AuthHandler(this);
}

Protocol::Response RequestHandler::handleRequest(const Protocol::Request &request) {
    // Обрабатываем запрос в зависимости от его типа
    switch (request.type()) {
        case Protocol::RequestType::AUTH_LOGIN:
            return m_authHandler->handleLogin(request);
            
        case Protocol::RequestType::AUTH_REGISTER:
            return m_authHandler->handleRegister(request);
            
        case Protocol::RequestType::TRIPLE_DES_ENCRYPT:
            return handleTripleDESEncrypt(request);
            
        case Protocol::RequestType::TRIPLE_DES_DECRYPT:
            return handleTripleDESDecrypt(request);
            
        case Protocol::RequestType::CHORD_METHOD_SOLVE:
            return handleChordMethodSolve(request);
            
        case Protocol::RequestType::STEGANOGRAPHY_HIDE:
            return handleSteganographyHide(request);
            
        case Protocol::RequestType::STEGANOGRAPHY_EXTRACT:
            return handleSteganographyExtract(request);
            
        default:
            // Неизвестный тип запроса
            Protocol::Response response(Protocol::ResponseStatus::ERROR);
            response.setErrorMessage("Неизвестный тип запроса");
            return response;
    }
}

Protocol::Response RequestHandler::handleTripleDESEncrypt(const Protocol::Request &request) {
    // Получаем данные из запроса
    QString text = request.data("text").toString();
    QByteArray key1 = QByteArray::fromBase64(request.data("key1").toString().toUtf8());
    QByteArray key2 = QByteArray::fromBase64(request.data("key2").toString().toUtf8());
    QByteArray key3 = QByteArray::fromBase64(request.data("key3").toString().toUtf8());
    
    // Проверка на пустые поля
    if (text.isEmpty() || key1.isEmpty() || key2.isEmpty() || key3.isEmpty()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Текст и ключи не могут быть пустыми");
        return response;
    }
    
    // Создаем объект Triple DES и устанавливаем ключи
    TripleDES des;
    des.setKeys(key1, key2, key3);
    
    // Шифруем текст
    QString encryptedText = des.encryptToBase64(text);
    
    // Формируем ответ
    Protocol::Response response(Protocol::ResponseStatus::SUCCESS);
    response.setData("encrypted_text", encryptedText);
    
    return response;
}

Protocol::Response RequestHandler::handleTripleDESDecrypt(const Protocol::Request &request) {
    // Получаем данные из запроса
    QString encryptedText = request.data("encrypted_text").toString();
    QByteArray key1 = QByteArray::fromBase64(request.data("key1").toString().toUtf8());
    QByteArray key2 = QByteArray::fromBase64(request.data("key2").toString().toUtf8());
    QByteArray key3 = QByteArray::fromBase64(request.data("key3").toString().toUtf8());
    
    // Проверка на пустые поля
    if (encryptedText.isEmpty() || key1.isEmpty() || key2.isEmpty() || key3.isEmpty()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Зашифрованный текст и ключи не могут быть пустыми");
        return response;
    }
    
    // Проверка формата Base64
    QByteArray testDecode = QByteArray::fromBase64(encryptedText.toUtf8());
    if (testDecode.isEmpty() && !encryptedText.isEmpty()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Зашифрованный текст должен быть в формате Base64");
        return response;
    }
    
    // Создаем объект Triple DES и устанавливаем ключи
    TripleDES des;
    des.setKeys(key1, key2, key3);
    
    // Дешифруем текст
    QString decryptedText;
    try {
        decryptedText = des.decryptFromBase64(encryptedText);
        
        // Проверяем, что результат дешифрования содержит только допустимые символы
        bool validText = true;
        for (const QChar &ch : decryptedText) {
            // Разрешаем все печатные символы, пробелы, переводы строк и табуляцию
            // Также разрешаем все символы Unicode (для поддержки разных языков)
            if (!ch.isPrint() && !ch.isSpace() && ch != '\n' && ch != '\r' && ch != '\t' && 
                ch.unicode() < 128 && ch.unicode() != 0) {
                validText = false;
                break;
            }
        }
        
        if (!validText) {
            Protocol::Response response(Protocol::ResponseStatus::ERROR);
            response.setErrorMessage("Результат дешифрования содержит недопустимые символы. Проверьте правильность ключей");
            return response;
        }
    } catch (const std::exception &e) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage(QString("Ошибка дешифрования: %1").arg(e.what()));
        return response;
    } catch (...) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Ошибка дешифрования. Проверьте правильность ключей и формат данных");
        return response;
    }
    
    // Формируем ответ
    Protocol::Response response(Protocol::ResponseStatus::SUCCESS);
    response.setData("decrypted_text", decryptedText);
    
    return response;
}

Protocol::Response RequestHandler::handleChordMethodSolve(const Protocol::Request &request) {
    // Получаем данные из запроса
    QString equation = request.data("equation").toString();
    double a = request.data("a").toDouble();
    double b = request.data("b").toDouble();
    double epsilon = request.data("epsilon").toDouble(0.0001);
    int maxIterations = request.data("max_iterations").toInt(100);
    
    // Проверка на пустые поля
    if (equation.isEmpty()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Уравнение не может быть пустым");
        return response;
    }
    
    // Проверка на корректные значения
    if (qFuzzyCompare(a, b)) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Значения a и b не могут быть равны");
        return response;
    }
    
    if (epsilon <= 0) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Точность (epsilon) должна быть положительной");
        return response;
    }
    
    if (maxIterations <= 0) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Максимальное число итераций должно быть положительным");
        return response;
    }
    
    // Решаем уравнение методом хорд
    ChordMethod::Result result = ChordMethod::solveEquation(equation, a, b, epsilon, maxIterations);
    
    // Формируем ответ
    if (result.success) {
        Protocol::Response response(Protocol::ResponseStatus::SUCCESS);
        response.setData("root", result.root);
        response.setData("iterations", result.iterations);
        response.setData("precision", result.precision);
        response.setData("status", static_cast<int>(Protocol::ResponseStatus::SUCCESS));
        return response;
    } else {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage(result.errorMessage);
        response.setData("status", static_cast<int>(Protocol::ResponseStatus::ERROR));
        return response;
    }
}

Protocol::Response RequestHandler::handleSteganographyHide(const Protocol::Request &request) {
    // Получаем данные из запроса
    QByteArray imageData = QByteArray::fromBase64(request.data("image").toString().toUtf8());
    QString message = request.data("message").toString();
    
    // Проверка на пустые поля
    if (imageData.isEmpty() || message.isEmpty()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Изображение и сообщение не могут быть пустыми");
        return response;
    }
    
    // Проверка размера сообщения
    if (message.length() > 10000) { // Ограничение на размер сообщения
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Сообщение слишком длинное. Максимальная длина: 10000 символов");
        return response;
    }
    
    // Проверка на допустимые символы в сообщении
    for (const QChar &ch : message) {
        if (!ch.isPrint() && !ch.isSpace() && ch != '\n' && ch != '\r' && ch != '\t') {
            Protocol::Response response(Protocol::ResponseStatus::ERROR);
            response.setErrorMessage("Сообщение содержит недопустимые символы");
            return response;
        }
    }
    
    // Загружаем изображение из данных
    QImage image;
    bool loadSuccess = image.loadFromData(imageData);
    
    if (!loadSuccess) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Не удалось загрузить изображение");
        return response;
    }
    
    // Проверка размера изображения
    if (image.width() < 10 || image.height() < 10 || image.width() * image.height() < 100) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Изображение слишком маленькое");
        return response;
    }
    
    try {
        // Скрываем сообщение в изображении
        Steganography::Result result = Steganography::hideMessage(image, message);
        
        // Формируем ответ
        if (result.success) {
            Protocol::Response response(Protocol::ResponseStatus::SUCCESS);
            // Сохраняем изображение в формате PNG
            QByteArray resultImageData;
            QBuffer buffer(&resultImageData);
            buffer.open(QIODevice::WriteOnly);
            
            if (!result.image.save(&buffer, "PNG")) {
                Protocol::Response errorResponse(Protocol::ResponseStatus::ERROR);
                errorResponse.setErrorMessage("Не удалось сохранить изображение");
                return errorResponse;
            }
            
            response.setData("image", QString(resultImageData.toBase64()));
            response.setData("message", result.message);
            response.setData("status", static_cast<int>(Protocol::ResponseStatus::SUCCESS));
            return response;
        } else {
            Protocol::Response response(Protocol::ResponseStatus::ERROR);
            response.setErrorMessage(result.message);
            response.setData("status", static_cast<int>(Protocol::ResponseStatus::ERROR));
            return response;
        }
    } catch (const std::exception &e) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage(QString("Ошибка при скрытии сообщения: %1").arg(e.what()));
        return response;
    } catch (...) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Неизвестная ошибка при скрытии сообщения");
        return response;
    }
}

Protocol::Response RequestHandler::handleSteganographyExtract(const Protocol::Request &request) {
    // Получаем данные из запроса
    QByteArray imageData = QByteArray::fromBase64(request.data("image").toString().toUtf8());
    
    // Проверка на пустые поля
    if (imageData.isEmpty()) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Изображение не может быть пустым");
        return response;
    }
    
    // Загружаем изображение из данных
    QImage image;
    bool loadSuccess = image.loadFromData(imageData);
    
    if (!loadSuccess) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Не удалось загрузить изображение");
        return response;
    }
    
    // Проверка размера изображения
    if (image.width() < 10 || image.height() < 10 || image.width() * image.height() < 100) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Изображение слишком маленькое");
        return response;
    }
    
    try {
        // Извлекаем сообщение из изображения
        Steganography::Result result = Steganography::extractMessage(image);
        
        // Формируем ответ
        if (result.success) {
            Protocol::Response response(Protocol::ResponseStatus::SUCCESS);
            response.setData("message", result.message);
            response.setData("status", static_cast<int>(Protocol::ResponseStatus::SUCCESS));
            return response;
        } else {
            Protocol::Response response(Protocol::ResponseStatus::ERROR);
            response.setErrorMessage(result.message);
            response.setData("status", static_cast<int>(Protocol::ResponseStatus::ERROR));
            return response;
        }
    } catch (const std::exception &e) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage(QString("Ошибка при извлечении сообщения: %1").arg(e.what()));
        return response;
    } catch (...) {
        Protocol::Response response(Protocol::ResponseStatus::ERROR);
        response.setErrorMessage("Неизвестная ошибка при извлечении сообщения");
        return response;
    }
} 