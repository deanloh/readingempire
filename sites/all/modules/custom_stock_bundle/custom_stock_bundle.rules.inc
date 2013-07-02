<?php
/**
* Rules to handle stock adjustments on session bundles
*/
function custom_stock_bundle_rules_action_info() {
  $actions = array();
 
  $actions['custom_stock_bundle_decrease_by_line_item'] = array(
    'label' => t('Decrease the stock level for all referenced products that apply, given a line item'),
    'group' => t('Commerce Stock'),
    'parameter' => array(
      'commerce_line_item' => array(
        'type' => 'commerce_line_item',
        'label' => t('Line item')
      ),
    ),
  );
 
  return $actions;
}

/**
* Altered version of commerce_stock_decrease_by_line_item() to find referenced products
*
* @param $line_item
*   A line item object.
*/
function custom_stock_bundle_decrease_by_line_item($line_item) {
    // load commerce_stock.rules.inc to get commerce_stock_adjust()
    module_load_include('inc', 'commerce_stock', 'commerce_stock.rules');
    dsm($line_item->type);
    if ($line_item->type == 'product') {
        // The product SKU that will have its stock level adjusted.
        $sku = $line_item->line_item_label;
        $product_bundle = commerce_product_load_by_sku($sku);
        // check product for our reference fields then check each product
        // field_bundle_sessions is the name of my product Reference field, you'll need to edit this to match your Product Bundle type
        if(isset ($product_bundle->field_product_references)) {
            foreach ($product_bundle->field_product_references['und'] as $products) {
                // load the reference product
                $product = commerce_product_load($products['product_id']);
                // after this, it works just the same as commerce_stock_decrease_by_line_item()
                if (commerce_stock_product_type_enabled($product->type)) {
                    if (!(commerce_stock_product_type_override_enabled($product->type) && $product->commerce_stock_override['und'][0]['value'] == 1)) {
                        $qty = (int) $line_item->quantity;
                        // Subtract the sold amount from the available stock level.
                        commerce_stock_adjust($product, -$qty);
                        //drupal_set_message("Decrementing stock value for product: ".$product->title);
                    }
                }
            } // end foreach
        }
    }
}
