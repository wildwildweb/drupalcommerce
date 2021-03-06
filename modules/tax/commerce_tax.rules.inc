<?php
//$Id$

/**
 * @file
 * Rules integration for line items.
 *
 * @addtogroup rules
 * @{
 */


/**
 * Implements hook_rules_event_info().
 *
 * Taxes that only need product information for calculation can be applied to a
 * line item when the sell price of a product is calculated, but that won't
 * accommodate calculating taxes for non-product line items or for tax types
 * that require the order context for the line item for calculation. Thus the
 * Tax module defines an additional event that calculates taxes with the order
 * passed as a parameter.
 */
function commerce_tax_rules_event_info() {
  $items = array();

  $items['commerce_tax_calculate_order'] = array(
    'label' => t('Calculating taxes for a line item on an order'),
    'group' => t('Commerce Tax'),
    'variables' => array(
      'line_item' => array(
        'label' => t('Line item'),
        'type' => 'commerce_line_item',
        'skip save' => TRUE,
      ),
      'order' => array(
        'label' => t('Order', array(), array('context' => 'a drupal commerce order')),
        'type' => 'commerce_order',
        'skip save' => TRUE,
      ),
    ),
    'access callback' => 'commerce_order_rules_access',
  );

  return $items;
}

/**
 * Implements hook_rules_action_info().
 */
function commerce_tax_rules_action_info() {
  $actions = array();

  if (count(commerce_tax_rates()) > 0) {
    $actions['commerce_tax_rate_apply'] = array(
      'label' => t('Apply a tax rate to a line item'),
      'parameter' => array(
        'line_item' => array(
          'type' => 'commerce_line_item',
          'label' => t('Line item'),
        ),
        'tax_rate_name' => array(
          'type' => 'text',
          'label' => t('Tax rate'),
          'options list' => 'commerce_tax_rate_titles',
        ),
      ),
      'group' => t('Commerce Tax'),
      'callbacks' => array(
        'execute' => 'commerce_tax_rate_rules_apply',
      ),
    );
  }

  if (count(commerce_tax_types()) > 0) {
    $actions['commerce_tax_calculate_by_type'] = array(
      'label' => t('Calculate taxes for a line item'),
      'parameter' => array(
        'line_item' => array(
          'type' => 'commerce_line_item',
          'label' => t('Line item'),
        ),
        'tax_type_name' => array(
          'type' => 'text',
          'label' => t('Tax type'),
          'options list' => 'commerce_tax_type_titles',
        ),
      ),
    );

    $actions['commerce_tax_calculate_order_by_type'] = array(
      'label' => t('Calculate taxes for a line item using its order'),
      'parameter' => array(
        'line_item' => array(
          'type' => 'commerce_line_item',
          'label' => t('Line item'),
        ),
        'order' => array(
           'type' => 'commerce_order',
           'label' => t('Order', array(), array('context' => 'a drupal commerce order')),
        ),
        'tax_type_name' => array(
          'type' => 'text',
          'label' => t('Tax type'),
          'options list' => 'commerce_tax_type_order_context_titles',
        ),
      ),
    );
  }

  return $actions;
}

/**
 * Rules action: loads and applies a tax rate to the given line item.
 */
function commerce_tax_rate_rules_apply($line_item, $name) {
  if ($tax_rate = commerce_tax_rate_load($name)) {
    commerce_tax_rate_apply($tax_rate, $line_item);
  }
}

/**
 * Rules action: checks for the application of each tax that requires only the
 *   line item to determine applicability.
 */
function commerce_tax_calculate_by_type($line_item, $tax_type_name) {
  if ($tax_type = commerce_tax_type_load($tax_type_name)) {
    commerce_tax_type_calculate_rates($tax_type, $line_item);
  }
}

/**
 * Rules action: checks for the application of each tax that requires the line
 *   item and order context to determine applicability.
 */
function commerce_tax_calculate_order_by_type($line_item, $order, $tax_type_name) {
  if ($tax_type = commerce_tax_type_load($tax_type_name)) {
    commerce_tax_type_calculate_rates($tax_type, $line_item, $order);
  }
}

/**
 * @}
 */
