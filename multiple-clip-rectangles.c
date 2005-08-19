        cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
        cairo_identity_matrix (cr);
        cairo_rectangle (cr, 0.0, 0.0, width, height);
        cairo_rectangle (cr, 200.0, 200.0, 100.0, 100.0);
        cairo_clip      (cr);
        cairo_new_path  (cr);

        cairo_translate (cr, 0, 0);
        cairo_rectangle (cr, 0, 0, width, height);
        cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
        cairo_fill (cr);

