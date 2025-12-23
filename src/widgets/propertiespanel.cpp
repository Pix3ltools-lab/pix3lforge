#include "propertiespanel.h"
#include <QScrollArea>

PropertiesPanel::PropertiesPanel(QWidget *parent)
    : QDockWidget(tr("Properties"), parent)
{
    // Create main widget and layout
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Create sections - buttons first so they're always visible
    createButtons(mainLayout);
    createBasicAdjustments(mainLayout);
    createColorAdjustments(mainLayout);

    // Add spacer at the end
    mainLayout->addStretch();

    // Make the panel scrollable
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(mainWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setWidget(scrollArea);
    setMinimumWidth(250);
}

void PropertiesPanel::createBasicAdjustments(QVBoxLayout *mainLayout)
{
    QGroupBox *basicGroup = new QGroupBox(tr("Basic Adjustments"), this);
    QVBoxLayout *groupLayout = new QVBoxLayout(basicGroup);
    groupLayout->setSpacing(5);

    // Brightness
    QWidget *brightnessWidget = createSliderControl(tr("Brightness"), &brightnessSlider, -100, 100, 0);
    groupLayout->addWidget(brightnessWidget);
    connect(brightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        brightnessValueLabel->setText(QString::number(value));
        emit brightnessChanged(value);
    });

    // Contrast
    QWidget *contrastWidget = createSliderControl(tr("Contrast"), &contrastSlider, -100, 100, 0);
    groupLayout->addWidget(contrastWidget);
    connect(contrastSlider, &QSlider::valueChanged, this, [this](int value) {
        contrastValueLabel->setText(QString::number(value));
        emit contrastChanged(value);
    });

    // Saturation
    QWidget *saturationWidget = createSliderControl(tr("Saturation"), &saturationSlider, -100, 100, 0);
    groupLayout->addWidget(saturationWidget);
    connect(saturationSlider, &QSlider::valueChanged, this, [this](int value) {
        saturationValueLabel->setText(QString::number(value));
        emit saturationChanged(value);
    });

    // Hue
    QWidget *hueWidget = createSliderControl(tr("Hue"), &hueSlider, -180, 180, 0);
    groupLayout->addWidget(hueWidget);
    connect(hueSlider, &QSlider::valueChanged, this, [this](int value) {
        hueValueLabel->setText(QString::number(value));
        emit hueChanged(value);
    });

    // Gamma
    QWidget *gammaWidget = createDoubleSpinControl(tr("Gamma"), &gammaSpinBox, 0.1, 10.0, 1.0, 0.1);
    groupLayout->addWidget(gammaWidget);
    connect(gammaSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::gammaChanged);

    mainLayout->addWidget(basicGroup);
}

void PropertiesPanel::createColorAdjustments(QVBoxLayout *mainLayout)
{
    QGroupBox *colorGroup = new QGroupBox(tr("Color Adjustments"), this);
    QVBoxLayout *groupLayout = new QVBoxLayout(colorGroup);
    groupLayout->setSpacing(5);

    // Temperature
    QWidget *temperatureWidget = createSliderControl(tr("Temperature"), &temperatureSlider, -100, 100, 0);
    groupLayout->addWidget(temperatureWidget);
    connect(temperatureSlider, &QSlider::valueChanged, this, [this](int value) {
        temperatureValueLabel->setText(QString::number(value));
        emit temperatureChanged(value);
    });

    // Exposure
    QWidget *exposureWidget = createSliderControl(tr("Exposure"), &exposureSlider, -100, 100, 0);
    groupLayout->addWidget(exposureWidget);
    connect(exposureSlider, &QSlider::valueChanged, this, [this](int value) {
        exposureValueLabel->setText(QString::number(value));
        emit exposureChanged(value);
    });

    // Shadows
    QWidget *shadowsWidget = createSliderControl(tr("Shadows"), &shadowsSlider, -100, 100, 0);
    groupLayout->addWidget(shadowsWidget);
    connect(shadowsSlider, &QSlider::valueChanged, this, [this](int value) {
        shadowsValueLabel->setText(QString::number(value));
        emit shadowsChanged(value);
    });

    // Highlights
    QWidget *highlightsWidget = createSliderControl(tr("Highlights"), &highlightsSlider, -100, 100, 0);
    groupLayout->addWidget(highlightsWidget);
    connect(highlightsSlider, &QSlider::valueChanged, this, [this](int value) {
        highlightsValueLabel->setText(QString::number(value));
        emit highlightsChanged(value);
    });

    mainLayout->addWidget(colorGroup);
}

void PropertiesPanel::createButtons(QVBoxLayout *mainLayout)
{
    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 10);

    applyButton = new QPushButton(tr("Apply"), this);
    applyButton->setToolTip(tr("Apply all adjustments to image (creates undo point)"));
    applyButton->setMinimumHeight(30);
    connect(applyButton, &QPushButton::clicked, this, &PropertiesPanel::applyAdjustments);

    resetButton = new QPushButton(tr("Reset"), this);
    resetButton->setToolTip(tr("Reset all adjustments to default"));
    resetButton->setMinimumHeight(30);
    connect(resetButton, &QPushButton::clicked, this, &PropertiesPanel::resetAll);

    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(resetButton);

    mainLayout->addWidget(buttonWidget);
}

QWidget* PropertiesPanel::createSliderControl(const QString &label, QSlider **slider,
                                               int min, int max, int defaultValue)
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 0, 0, 0);

    // Label with value
    QWidget *labelWidget = new QWidget(this);
    QHBoxLayout *labelLayout = new QHBoxLayout(labelWidget);
    labelLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *nameLabel = new QLabel(label + ":", this);
    QLabel *valueLabel = new QLabel(QString::number(defaultValue), this);
    valueLabel->setMinimumWidth(35);
    valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    labelLayout->addWidget(nameLabel);
    labelLayout->addStretch();
    labelLayout->addWidget(valueLabel);

    layout->addWidget(labelWidget);

    // Slider
    *slider = new QSlider(Qt::Horizontal, this);
    (*slider)->setRange(min, max);
    (*slider)->setValue(defaultValue);
    (*slider)->setTickPosition(QSlider::TicksBelow);
    (*slider)->setTickInterval((max - min) / 10);

    layout->addWidget(*slider);

    // Store value label for updates
    if (label == tr("Brightness"))
        brightnessValueLabel = valueLabel;
    else if (label == tr("Contrast"))
        contrastValueLabel = valueLabel;
    else if (label == tr("Saturation"))
        saturationValueLabel = valueLabel;
    else if (label == tr("Hue"))
        hueValueLabel = valueLabel;
    else if (label == tr("Temperature"))
        temperatureValueLabel = valueLabel;
    else if (label == tr("Exposure"))
        exposureValueLabel = valueLabel;
    else if (label == tr("Shadows"))
        shadowsValueLabel = valueLabel;
    else if (label == tr("Highlights"))
        highlightsValueLabel = valueLabel;

    return widget;
}

QWidget* PropertiesPanel::createDoubleSpinControl(const QString &label, QDoubleSpinBox **spinBox,
                                                   double min, double max, double defaultValue, double step)
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *nameLabel = new QLabel(label + ":", this);
    layout->addWidget(nameLabel);

    *spinBox = new QDoubleSpinBox(this);
    (*spinBox)->setRange(min, max);
    (*spinBox)->setValue(defaultValue);
    (*spinBox)->setSingleStep(step);
    (*spinBox)->setDecimals(1);

    layout->addWidget(*spinBox);

    return widget;
}

void PropertiesPanel::resetAll()
{
    // Block signals to prevent multiple updates
    brightnessSlider->blockSignals(true);
    contrastSlider->blockSignals(true);
    saturationSlider->blockSignals(true);
    hueSlider->blockSignals(true);
    gammaSpinBox->blockSignals(true);
    temperatureSlider->blockSignals(true);
    exposureSlider->blockSignals(true);
    shadowsSlider->blockSignals(true);
    highlightsSlider->blockSignals(true);

    // Reset all values
    brightnessSlider->setValue(0);
    contrastSlider->setValue(0);
    saturationSlider->setValue(0);
    hueSlider->setValue(0);
    gammaSpinBox->setValue(1.0);
    temperatureSlider->setValue(0);
    exposureSlider->setValue(0);
    shadowsSlider->setValue(0);
    highlightsSlider->setValue(0);

    // Update labels
    brightnessValueLabel->setText("0");
    contrastValueLabel->setText("0");
    saturationValueLabel->setText("0");
    hueValueLabel->setText("0");
    temperatureValueLabel->setText("0");
    exposureValueLabel->setText("0");
    shadowsValueLabel->setText("0");
    highlightsValueLabel->setText("0");

    // Unblock signals
    brightnessSlider->blockSignals(false);
    contrastSlider->blockSignals(false);
    saturationSlider->blockSignals(false);
    hueSlider->blockSignals(false);
    gammaSpinBox->blockSignals(false);
    temperatureSlider->blockSignals(false);
    exposureSlider->blockSignals(false);
    shadowsSlider->blockSignals(false);
    highlightsSlider->blockSignals(false);

    emit resetAdjustments();
}

AdjustmentParameters PropertiesPanel::getAdjustments() const
{
    AdjustmentParameters params;
    params.brightness = getBrightness();
    params.contrast = getContrast();
    params.saturation = getSaturation();
    params.hue = getHue();
    params.gamma = getGamma();
    params.temperature = getColorTemperature();
    params.exposure = getExposure();
    params.shadows = getShadows();
    params.highlights = getHighlights();
    return params;
}

void PropertiesPanel::setAdjustments(const AdjustmentParameters &params)
{
    // Block signals to prevent multiple updates
    brightnessSlider->blockSignals(true);
    contrastSlider->blockSignals(true);
    saturationSlider->blockSignals(true);
    hueSlider->blockSignals(true);
    gammaSpinBox->blockSignals(true);
    temperatureSlider->blockSignals(true);
    exposureSlider->blockSignals(true);
    shadowsSlider->blockSignals(true);
    highlightsSlider->blockSignals(true);

    // Set all values
    brightnessSlider->setValue(params.brightness);
    contrastSlider->setValue(params.contrast);
    saturationSlider->setValue(params.saturation);
    hueSlider->setValue(params.hue);
    gammaSpinBox->setValue(params.gamma);
    temperatureSlider->setValue(params.temperature);
    exposureSlider->setValue(params.exposure);
    shadowsSlider->setValue(params.shadows);
    highlightsSlider->setValue(params.highlights);

    // Update labels
    brightnessValueLabel->setText(QString::number(params.brightness));
    contrastValueLabel->setText(QString::number(params.contrast));
    saturationValueLabel->setText(QString::number(params.saturation));
    hueValueLabel->setText(QString::number(params.hue));
    temperatureValueLabel->setText(QString::number(params.temperature));
    exposureValueLabel->setText(QString::number(params.exposure));
    shadowsValueLabel->setText(QString::number(params.shadows));
    highlightsValueLabel->setText(QString::number(params.highlights));

    // Unblock signals
    brightnessSlider->blockSignals(false);
    contrastSlider->blockSignals(false);
    saturationSlider->blockSignals(false);
    hueSlider->blockSignals(false);
    gammaSpinBox->blockSignals(false);
    temperatureSlider->blockSignals(false);
    exposureSlider->blockSignals(false);
    shadowsSlider->blockSignals(false);
    highlightsSlider->blockSignals(false);
}
