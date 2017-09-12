#include <Implementations/SettingsGenerator.hpp>
#include <Implementations/SettingsStep.hpp>

SettingsGenerator::SettingsGenerator(Interfaces::IAdcTemperatureConvertor* convertor)
{
  this->_ADC2TempConvertor = convertor;

  // Setting up steps number
  this->InitializeStepsList(STEPS_NUMBER);
}

void SettingsGenerator::InitializeStepsList(uint steps_number)
{
  // Destroying old ones
  this->DestroySteps();

  this->_steps.clear();

  // And creating a new ones
  for (uint i = 0; i < steps_number + ADDITIONAL_STEPS; i++)
  {
    SettingsStep* step = new SettingsStep(this->_ADC2TempConvertor);
    this->_steps.push_back(step);
  }

  // Calculating steps data
  this->CalculateSteps();
}

void SettingsGenerator::SetBaseTemperature(double btemp)
{
  this->_baseADC = this->_ADC2TempConvertor->TEMP2ADC(btemp);

  this->CalculateSteps(); // Recalculating
}

double SettingsGenerator::GetBaseTemperature()
{
  return this->_ADC2TempConvertor->ADC2TEMP(this->_baseADC);
}

uint SettingsGenerator::GetBaseTemperatureADC()
{
  return this->_baseADC;
}

void SettingsGenerator::SetBaseRPM(uint base_rpm)
{
  this->_baseRPM = base_rpm;

  this->CalculateSteps(); // Recalculating
}

uint SettingsGenerator::GetBaseRPM()
{
  return this->_baseRPM;
}

void SettingsGenerator::DestroySteps()
{
  // Destroying all steps
  for (QList<Interfaces::ISettingsStep*>::iterator i = this->_steps.begin(); i != this->_steps.end(); ++i)
  {
    SafeDelete(*i);
  }
}

void SettingsGenerator::CalculateSteps()
{
  // Zero levels step
  Interfaces::ISettingsStep *stepPtr = this->_steps[ZERO_STEP_INDEX];

  stepPtr->SetADCDelta(0);
  stepPtr->SetRPMDelta(0);
  stepPtr->CalculatePrimaryValues(0, 0);

  // Base levels step
  stepPtr = this->_steps[BASE_STEP_INDEX];
  stepPtr->SetADCDelta(this->_baseADC, true);
  stepPtr->SetRPMDelta(this->_baseRPM, true);

  // Iterating through steps (except first)
  for (int i = 1; i < this->_steps.size(); i++)
  {
    // Previous element
    Interfaces::ISettingsStep* prev = this->_steps[i - 1];
    this->_steps[i]->CalculatePrimaryValues(prev->GetCurrentADC(), prev->GetCurrentRPM());
  }
}

Interfaces::ISettingsStep* SettingsGenerator::GetStepPtr(uint step)
{
  return this->_steps[step];
}

SettingsGenerator::~SettingsGenerator()
{
  this->DestroySteps();
}
