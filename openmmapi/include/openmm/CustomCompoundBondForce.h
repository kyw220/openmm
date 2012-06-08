#ifndef OPENMM_CUSTOMCOMPOUNDBONDFORCE_H_
#define OPENMM_CUSTOMCOMPOUNDBONDFORCE_H_

/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2008-2012 Stanford University and the Authors.      *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "Force.h"
#include "Vec3.h"
#include <vector>
#include "internal/windowsExport.h"

namespace OpenMM {

/**
 * This class supports a wide variety of bonded interactions.  It defines a "bond" as a single energy term
 * that depends on the positions of a fixed set of particles.  The number of particles involved in a bond, and how
 * the energy depends on their positions, is configurable.  It may depend on the positions of individual particles,
 * the distances between pairs of particles, the angles formed by sets of three particles, and the dihedral
 * angles formed by sets of four particles.
 * 
 * We refer to the particles in a bond as p1, p2, p3, etc.  For each bond, CustomCompoundBondForce evaluates a
 * user supplied algebraic expression to determine the interaction energy.  The expression may depend on the
 * following variables and functions:
 * 
 * <ul>
 * <li>x1, y1, z1, x2, y2, z2, etc.: The x, y, and z coordinates of the particle positions.  For example, x1
 * is the x coordinate of particle p1, and y3 is the y coordinate of particle p3.</li>
 * <li>distance(p1, p2): the distance between particles p1 and p2 (where "p1" and "p2" may be replaced by the names
 * of whichever particles you want to calculate the distance between).</li>
 * <li>angle(p1, p2, p3): the angle formed by the three specified particles.</li>
 * <li>dihedral(p1, p2, p3, p4): the dihedral angle formed by the four specified particles.</li>
 * </ul>
 *
 * The expression also may involve tabulated functions, and may depend on arbitrary
 * global and per-bond parameters.
 *
 * To use this class, create a CustomCompoundBondForce object, passing an algebraic expression to the constructor
 * that defines the interaction energy of each bond.  Then call addPerBondParameter() to define per-bond
 * parameters and addGlobalParameter() to define global parameters.  The values of per-bond parameters are specified
 * as part of the system definition, while values of global parameters may be modified during a simulation by calling
 * Context::setParameter().
 *
 * Next, call addBond() to define bonds and specify their parameter values.  After a bond has been added, you can
 * modify its parameters by calling setBondParameters().
 *
 * As an example, the following code creates a CustomCompoundBondForce that implements a Urey-Bradley potential.  This
 * is an interaction between three particles that depends on the angle formed by p1-p2-p3, and on the distance between
 * p1 and p3.
 *
 * <tt>CustomCompoundBondForce* force = new CustomCompoundBondForce(3, "0.5*(kangle*(angle(p1,p2,p3)-theta0)^2+kbond*(distance(p1,p3)-r0)^2)");</tt>
 *
 * This force depends on four parameters: kangle, kbond, theta0, and r0.  The following code defines these as per-bond parameters:
 *
 * <tt><pre>
 * force->addPerBondParameter("kangle");
 * force->addPerBondParameter("kbond");
 * force->addPerBondParameter("theta0");
 * force->addPerBondParameter("r0");
 * </pre></tt>
 *
 * Expressions may involve the operators + (add), - (subtract), * (multiply), / (divide), and ^ (power), and the following
 * functions: sqrt, exp, log, sin, cos, sec, csc, tan, cot, asin, acos, atan, sinh, cosh, tanh, erf, erfc, min, max, abs, step, delta.  All trigonometric functions
 * are defined in radians, and log is the natural logarithm.  step(x) = 0 if x is less than 0, 1 otherwise.  delta(x) = 1 if x is 0, 0 otherwise.
 *
 * In addition, you can call addFunction() to define a new function based on tabulated values.  You specify a vector of
 * values, and a natural spline is created from them.  That function can then appear in the expression.
 */

class OPENMM_EXPORT CustomCompoundBondForce : public Force {
public:
    /**
     * Create a CustomCompoundBondForce.
     *
     * @param numParticles  the number of particles used to define each bond
     * @param energy        an algebraic expression giving the interaction energy of each bond as a function
     *                      of particle positions, inter-particle distances, angles, and dihedrals, and any global
     *                      and per-bond parameters
     */
    explicit CustomCompoundBondForce(int numParticles, const std::string& energy);
    /**
     * Get the number of particles used to define each bond.
     */
    int getNumParticlesPerBond() const {
        return particlesPerBond;
    }
    /**
     * Get the number of bonds for which force field parameters have been defined.
     */
    int getNumBonds() const {
        return bonds.size();
    }
    /**
     * Get the number of per-bond parameters that the interaction depends on.
     */
    int getNumPerBondParameters() const {
        return bondParameters.size();
    }
    /**
     * Get the number of global parameters that the interaction depends on.
     */
    int getNumGlobalParameters() const {
        return globalParameters.size();
    }
    /**
     * Get the number of tabulated functions that have been defined.
     */
    int getNumFunctions() const {
        return functions.size();
    }
    /**
     * Get the algebraic expression that gives the interaction energy of each bond
     */
    const std::string& getEnergyFunction() const;
    /**
     * Set the algebraic expression that gives the interaction energy of each bond
     */
    void setEnergyFunction(const std::string& energy);
    /**
     * Add a new per-bond parameter that the interaction may depend on.
     *
     * @param name     the name of the parameter
     * @return the index of the parameter that was added
     */
    int addPerBondParameter(const std::string& name);
    /**
     * Get the name of a per-bond parameter.
     *
     * @param index     the index of the parameter for which to get the name
     * @return the parameter name
     */
    const std::string& getPerBondParameterName(int index) const;
    /**
     * Set the name of a per-bond parameter.
     *
     * @param index          the index of the parameter for which to set the name
     * @param name           the name of the parameter
     */
    void setPerBondParameterName(int index, const std::string& name);
    /**
     * Add a new global parameter that the interaction may depend on.
     *
     * @param name             the name of the parameter
     * @param defaultValue     the default value of the parameter
     * @return the index of the parameter that was added
     */
    int addGlobalParameter(const std::string& name, double defaultValue);
    /**
     * Get the name of a global parameter.
     *
     * @param index     the index of the parameter for which to get the name
     * @return the parameter name
     */
    const std::string& getGlobalParameterName(int index) const;
    /**
     * Set the name of a global parameter.
     *
     * @param index          the index of the parameter for which to set the name
     * @param name           the name of the parameter
     */
    void setGlobalParameterName(int index, const std::string& name);
    /**
     * Get the default value of a global parameter.
     *
     * @param index     the index of the parameter for which to get the default value
     * @return the parameter default value
     */
    double getGlobalParameterDefaultValue(int index) const;
    /**
     * Set the default value of a global parameter.
     *
     * @param index          the index of the parameter for which to set the default value
     * @param name           the default value of the parameter
     */
    void setGlobalParameterDefaultValue(int index, double defaultValue);
    /**
     * Add a bond to the force
     *
     * @param particles   the indices of the particles the bond depends on
     * @param parameters  the list of per-bond parameter values for the new bond
     * @return the index of the bond that was added
     */
    int addBond(const std::vector<int>& particles, const std::vector<double>& parameters);
    /**
     * Get the properties of a bond.
     *
     * @param index       the index of the bond to get
     * @param particles   the indices of the particles in the bond
     * @param parameters  the list of per-bond parameter values for the bond
     */
    void getBondParameters(int index, std::vector<int>& particles, std::vector<double>& parameters) const;
    /**
     * Set the properties of a bond.
     *
     * @param index       the index of the bond group to set
     * @param particles   the indices of the particles in the bond
     * @param parameters  the list of per-bond parameter values for the bond
     */
    void setBondParameters(int index, const std::vector<int>& particles, const std::vector<double>& parameters);
    /**
     * Add a tabulated function that may appear in the energy expression.
     *
     * @param name           the name of the function as it appears in expressions
     * @param values         the tabulated values of the function f(x) at uniformly spaced values of x between min and max.
     *                       The function is assumed to be zero for x &lt; min or x &gt; max.
     * @param min            the value of the independent variable corresponding to the first element of values
     * @param max            the value of the independent variable corresponding to the last element of values
     * @return the index of the function that was added
     */
    int addFunction(const std::string& name, const std::vector<double>& values, double min, double max);
    /**
     * Get the parameters for a tabulated function that may appear in the energy expression.
     *
     * @param index          the index of the function for which to get parameters
     * @param name           the name of the function as it appears in expressions
     * @param values         the tabulated values of the function f(x) at uniformly spaced values of x between min and max.
     *                       The function is assumed to be zero for x &lt; min or x &gt; max.
     * @param min            the value of the independent variable corresponding to the first element of values
     * @param max            the value of the independent variable corresponding to the last element of values
     */
    void getFunctionParameters(int index, std::string& name, std::vector<double>& values, double& min, double& max) const;
    /**
     * Set the parameters for a tabulated function that may appear in algebraic expressions.
     *
     * @param index          the index of the function for which to set parameters
     * @param name           the name of the function as it appears in expressions
     * @param values         the tabulated values of the function f(x) at uniformly spaced values of x between min and max.
     *                       The function is assumed to be zero for x &lt; min or x &gt; max.
     * @param min            the value of the independent variable corresponding to the first element of values
     * @param max            the value of the independent variable corresponding to the last element of values
     */
    void setFunctionParameters(int index, const std::string& name, const std::vector<double>& values, double min, double max);
    /**
     * Update the per-bond parameters in a Context to match those stored in this Force object.  This method provides
     * an efficient method to update certain parameters in an existing Context without needing to reinitialize it.
     * Simply call setBondParameters() to modify this object's parameters, then call updateParametersInState()
     * to copy them over to the Context.
     * 
     * This method has several limitations.  The only information it updates is the values of per-bond parameters.
     * All other aspects of the Force (such as the energy function) are unaffected and can only be changed by reinitializing
     * the Context.  The set of particles involved in a bond cannot be changed, nor can new bonds be added.
     */
    void updateParametersInContext(Context& context);
protected:
    ForceImpl* createImpl();
private:
    class BondInfo;
    class BondParameterInfo;
    class GlobalParameterInfo;
    class FunctionInfo;
    int particlesPerBond;
    std::string energyExpression;
    std::vector<BondParameterInfo> bondParameters;
    std::vector<GlobalParameterInfo> globalParameters;
    std::vector<BondInfo> bonds;
    std::vector<FunctionInfo> functions;
};

/**
 * This is an internal class used to record information about a bond or acceptor.
 * @private
 */
class CustomCompoundBondForce::BondInfo {
public:
    std::vector<int> particles;
    std::vector<double> parameters;
    BondInfo() {
    }
    BondInfo(const std::vector<int>& particles, const std::vector<double>& parameters) :
        particles(particles), parameters(parameters) {
    }
};

/**
 * This is an internal class used to record information about a per-bond or per-acceptor parameter.
 * @private
 */
class CustomCompoundBondForce::BondParameterInfo {
public:
    std::string name;
    BondParameterInfo() {
    }
    BondParameterInfo(const std::string& name) : name(name) {
    }
};

/**
 * This is an internal class used to record information about a global parameter.
 * @private
 */
class CustomCompoundBondForce::GlobalParameterInfo {
public:
    std::string name;
    double defaultValue;
    GlobalParameterInfo() {
    }
    GlobalParameterInfo(const std::string& name, double defaultValue) : name(name), defaultValue(defaultValue) {
    }
};

/**
 * This is an internal class used to record information about a tabulated function.
 * @private
 */
class CustomCompoundBondForce::FunctionInfo {
public:
    std::string name;
    std::vector<double> values;
    double min, max;
    FunctionInfo() {
    }
    FunctionInfo(const std::string& name, const std::vector<double>& values, double min, double max) :
        name(name), values(values), min(min), max(max) {
    }
};

} // namespace OpenMM

#endif /*OPENMM_CUSTOMCOMPOUNDBONDFORCE_H_*/