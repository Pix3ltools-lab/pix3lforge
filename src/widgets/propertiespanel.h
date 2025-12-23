#ifndef PROPERTIESPANEL_H
#define PROPERTIESPANEL_H

#include <QDockWidget>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include "../model/adjustmentparameters.h"

class PropertiesPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit PropertiesPanel(QWidget *parent = nullptr);

    // Get current adjustment values (individual accessors for backward compatibility)
    int getBrightness() const { return brightnessSlider->value(); }
    int getContrast() const { return contrastSlider->value(); }
    int getSaturation() const { return saturationSlider->value(); }
    int getHue() const { return hueSlider->value(); }
    double getGamma() const { return gammaSpinBox->value(); }
    int getColorTemperature() const { return temperatureSlider->value(); }
    int getExposure() const { return exposureSlider->value(); }
    int getShadows() const { return shadowsSlider->value(); }
    int getHighlights() const { return highlightsSlider->value(); }

    // Get all adjustments as a single value object (recommended)
    AdjustmentParameters getAdjustments() const;

    // Set adjustments from value object
    void setAdjustments(const AdjustmentParameters &params);

    // Reset all adjustments
    void resetAll();

signals:
    // Emitted when any slider changes (for live preview)
    void brightnessChanged(int value);
    void contrastChanged(int value);
    void saturationChanged(int value);
    void hueChanged(int value);
    void gammaChanged(double value);
    void temperatureChanged(int value);
    void exposureChanged(int value);
    void shadowsChanged(int value);
    void highlightsChanged(int value);

    // Emitted when Apply button is clicked
    void applyAdjustments();

    // Emitted when Reset button is clicked
    void resetAdjustments();

private:
    void createBasicAdjustments(QVBoxLayout *mainLayout);
    void createColorAdjustments(QVBoxLayout *mainLayout);
    void createButtons(QVBoxLayout *mainLayout);
    QWidget* createSliderControl(const QString &label, QSlider **slider,
                                  int min, int max, int defaultValue);
    QWidget* createDoubleSpinControl(const QString &label, QDoubleSpinBox **spinBox,
                                     double min, double max, double defaultValue, double step);

    // Basic adjustments
    QSlider *brightnessSlider;
    QSlider *contrastSlider;
    QSlider *saturationSlider;
    QSlider *hueSlider;
    QDoubleSpinBox *gammaSpinBox;

    // Color adjustments
    QSlider *temperatureSlider;
    QSlider *exposureSlider;
    QSlider *shadowsSlider;
    QSlider *highlightsSlider;

    // Buttons
    QPushButton *applyButton;
    QPushButton *resetButton;

    // Labels to show current values
    QLabel *brightnessValueLabel;
    QLabel *contrastValueLabel;
    QLabel *saturationValueLabel;
    QLabel *hueValueLabel;
    QLabel *temperatureValueLabel;
    QLabel *exposureValueLabel;
    QLabel *shadowsValueLabel;
    QLabel *highlightsValueLabel;
};

#endif // PROPERTIESPANEL_H
