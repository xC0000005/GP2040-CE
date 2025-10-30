import { useTranslation } from 'react-i18next';
import { FormCheck, Row } from 'react-bootstrap';
import * as yup from 'yup';

import Section from '../Components/Section';

import FormControl from '../Components/FormControl';
import { AddonPropTypes } from '../Pages/AddonsConfigPage';

export const legendsPlungerScheme = {
	LegendsPlungerAddonEnabled: yup
		.number()
		.required()
		.label('Legends Plunger Add-On Enabled'),
	legendsPlungerClockPin: yup
		.number()
		.label('Plunger Clock Pin')
		.validatePinWhenValue('LegendsPlungerAddonEnabled'),
	legendsPlungerDataPin: yup
		.number()
		.label('Plunger Data Pin')
		.validatePinWhenValue('LegendsPlungerAddonEnabled'),
};

export const legendsPlungerState = {
	LegendsPlungerAddonEnabled: 0,
	legendsPlungerClockPin: -1,
	legendsPlungerDataPin: -1,
};

const LegendsPlunger = ({ values, errors, handleChange, handleCheckbox }: AddonPropTypes) => {
	const { t } = useTranslation();
	return (
		<Section title={
			<a
				href="https://gp2040-ce.info/add-ons/legends-plunger"
				target="_blank"
				className="text-reset text-decoration-none"
			>
				{t('AddonsConfig:legends-plunger-header-text')}
			</a>
		}
		>
			<div
				id="LegendsPlungerAddonOptions"
				hidden={!values.LegendsPlungerAddonEnabled}
			>
				<Row className="mb-3">
					<FormControl
						type="number"
						label={t('AddonsConfig:legends-plunger-clock-pin-label')}
						name="legendsPlungerClockPin"
						className="form-control-sm"
						groupClassName="col-sm-3 mb-3"
						value={values.legendsPlungerClockPin}
						error={errors.legendsPlungerClockPin}
						isInvalid={Boolean(errors.legendsPlungerClockPin)}
						onChange={handleChange}
						min={-1}
						max={29}
					/>
					<FormControl
						type="number"
						label={t('AddonsConfig:legends-plunger-data-pin-label')}
						name="legendsPlungerDataPin"
						className="form-control-sm"
						groupClassName="col-sm-3 mb-3"
						value={values.legendsPlungerDataPin}
						error={errors.legendsPlungerDataPin}
						isInvalid={Boolean(errors.legendsPlungerDataPin)}
						onChange={handleChange}
						min={-1}
						max={29}
					/>
				</Row>
			</div>
			<FormCheck
				label={t('Common:switch-enabled')}
				type="switch"
				id="LegendsPlungerAddonButton"
				reverse
				isInvalid={false}
				checked={Boolean(values.LegendsPlungerAddonEnabled)}
				onChange={(e) => {
					handleCheckbox('LegendsPlungerAddonEnabled');
					handleChange(e);
				}}
			/>
		</Section>
	);
};

export default LegendsPlunger;
