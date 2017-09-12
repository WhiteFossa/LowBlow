#ifndef SETTINGSGENERATOR_HPP
#define SETTINGSGENERATOR_HPP

#include <Interfaces/ISettingsStep.hpp>
#include <Interfaces/ISettingsGenerator.hpp>
#include <Auxiliary.hpp>

/**
  * Number of steps. STEPS_NUMBER = EEPROM_SIZE - 3
  */
#define STEPS_NUMBER 125

/**
  * Actually we creating two steps more, than given in InitializeStepsList() to store zero levels step and base level step.
  */
#define ADDITIONAL_STEPS 2

/**
  STEPS_NUMBER + ADDITIONAL_STEPS
  */
#define NUMBER_OF_STEPS_TABLE_RECORDS (STEPS_NUMBER + ADDITIONAL_STEPS)

/**
  * Zero levels step index
  */
#define ZERO_STEP_INDEX 0

/**
  * Base levels step index
  */
#define BASE_STEP_INDEX 1


/**
 * @brief Class, being used to generate settings (see Interfaces/ISettingsGenerator for information)
 */
class SettingsGenerator : public Interfaces::ISettingsGenerator
{
  Q_OBJECT

  public:

    /**
     * @brief Constructor. Initializes steps list, default base temperature and so on.
     * @param convertor - Pointer to instance of AdcTemperatureConvertor class.
     */
    SettingsGenerator(Interfaces::IAdcTemperatureConvertor* convertor);

    /**
     * @brief Destructor
     */
    ~SettingsGenerator();

    void InitializeStepsList(uint steps_number);


    void SetBaseTemperature(double btemp);
    double GetBaseTemperature();
    uint GetBaseTemperatureADC();

    void SetBaseRPM(uint base_rpm);
    uint GetBaseRPM();

    void CalculateSteps();

    Interfaces::ISettingsStep* GetStepPtr(uint step);



  protected:

  private:

    Interfaces::IAdcTemperatureConvertor* _ADC2TempConvertor = NULL;

    /**
     * @brief _steps - List of pointers to RPM and temperature change steps.
     */
    QList <Interfaces::ISettingsStep*> _steps;

    /**
     * @brief _baseADC Base temperature ADC level.
     */
    uint _baseADC;

    /**
     * @brief _baseRPM - Base RPM level. See SetBaseRPM() method description in ISettingsGenerator for details.
     */
    uint _baseRPM;

    /**
     * @brief Call this method to destroy steps, contained in _steps. _steps size will not be changed, but pointers will be NULL.
     */
    void DestroySteps();

};

#endif // SETTINGSGENERATOR_HPP
