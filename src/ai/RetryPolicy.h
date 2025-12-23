#ifndef RETRYPOLICY_H
#define RETRYPOLICY_H

#include <functional>
#include <QThread>
#include <QDebug>

/**
 * Retry policy with exponential backoff
 * Executes a function with automatic retry on failure
 */
class RetryPolicy
{
public:
    /**
     * Create retry policy
     * @param maxRetries Maximum retry attempts
     * @param baseDelayMs Base delay in milliseconds
     * @param maxDelayMs Maximum delay in milliseconds (cap for exponential backoff)
     */
    RetryPolicy(int maxRetries, int baseDelayMs, int maxDelayMs)
        : m_maxRetries(maxRetries)
        , m_baseDelayMs(baseDelayMs)
        , m_maxDelayMs(maxDelayMs)
    {}

    /**
     * Execute function with retry logic
     * @param func Function that returns true on success, false on failure
     * @param onRetry Optional callback called before each retry (receives retry attempt number)
     * @return true if function eventually succeeded, false if all retries exhausted
     */
    bool execute(std::function<bool()> func, std::function<void(int)> onRetry = nullptr)
    {
        for (int attempt = 0; attempt <= m_maxRetries; ++attempt) {
            if (attempt > 0) {
                // Calculate exponential backoff delay
                int delay = m_baseDelayMs * (1 << (attempt - 1));  // 2^(attempt-1)
                delay = qMin(delay, m_maxDelayMs);

                qDebug() << "RetryPolicy: Waiting" << delay << "ms before retry" << attempt;
                QThread::msleep(delay);

                if (onRetry) {
                    onRetry(attempt);
                }
            }

            bool success = func();
            if (success) {
                if (attempt > 0) {
                    qDebug() << "RetryPolicy: Succeeded on retry attempt" << attempt;
                }
                return true;
            }
        }

        qWarning() << "RetryPolicy: Failed after" << m_maxRetries << "retries";
        return false;
    }

private:
    int m_maxRetries;
    int m_baseDelayMs;
    int m_maxDelayMs;
};

#endif // RETRYPOLICY_H
