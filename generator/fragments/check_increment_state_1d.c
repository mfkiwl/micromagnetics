/* Now we need to check if our neighbours satisfy the update condition. If they
 * do, we update our state. */
uint8_t goingToUpdate = 1;  /* Innocent until proven guilty. */
if (x0plus_exists) if (iteration_x0plus < iteration) goingToUpdate = 0;
if (x0minus_exists) if(iteration_x0minus < iteration) goingToUpdate = 0;
if (goingToUpdate == 1)
{
    /* Compute exchange field. */
    float h_eff_x0;
    float h_eff_x1;
    float h_eff_x2;
    float m_x0_x0plus;
    float m_x1_x0plus;
    float m_x2_x0plus;
    float m_x0_x0minus;
    float m_x1_x0minus;
    float m_x2_x0minus;

    /* Which buffer should we use? */
    if (iteration % 2)
    {
        m_x0_x0plus = m_x0_x0plus_odd;
        m_x1_x0plus = m_x1_x0plus_odd;
        m_x2_x0plus = m_x2_x0plus_odd;
        m_x0_x0minus = m_x0_x0minus_odd;
        m_x1_x0minus = m_x1_x0minus_odd;
        m_x2_x0minus = m_x2_x0minus_odd;
    }
    else
    {
        m_x0_x0plus = m_x0_x0plus_even;
        m_x1_x0plus = m_x1_x0plus_even;
        m_x2_x0plus = m_x2_x0plus_even;
        m_x0_x0minus = m_x0_x0minus_even;
        m_x1_x0minus = m_x1_x0minus_even;
        m_x2_x0minus = m_x2_x0minus_even;
    }

    /* Exchange effective field. */
    h_eff_x0 = exchange_coeff * (m_x0_x0plus + m_x0_x0minus);
    h_eff_x1 = exchange_coeff * (m_x1_x0plus + m_x1_x0minus);
    h_eff_x2 = exchange_coeff * (m_x2_x0plus + m_x2_x0minus);

    /* Stir in a little DMI. */
    h_eff_x1 += dmi_coeff * (m_x2_x0plus - m_x2_x0minus);
    h_eff_x2 += dmi_coeff * (m_x1_x0minus - m_x1_x0plus);

    /* Precessionless LLG (see p30 of MLV's thesis) */
    float dmdt_x0;
    float dmdt_x1;
    float dmdt_x2;
    float m_dot_h;

    m_dot_h = m_x0 * h_eff_x0 + m_x1 * h_eff_x1 + m_x2 * h_eff_x2;
    dmdt_x0 = m_dot_h * m_x0 - h_eff_x0;
    dmdt_x1 = m_dot_h * m_x1 - h_eff_x1;
    dmdt_x2 = m_dot_h * m_x2 - h_eff_x2;

    /* Step forward in time (Euler), with dt = 1e-13 seconds. */
    iteration++;
    m_x0 += 1e-13 * dmdt_x0;
    m_x1 += 1e-13 * dmdt_x1;
    m_x2 += 1e-13 * dmdt_x2;

    /* We did it! This variable notifies the ReadyToSend (RTS) handler that
     * we've just updated. Once read, the RTS handler will set this to zero, so
     * that successive RTSs don't send a message unless a new state can be
     * computed. */
    did_we_just_update = 1;
}