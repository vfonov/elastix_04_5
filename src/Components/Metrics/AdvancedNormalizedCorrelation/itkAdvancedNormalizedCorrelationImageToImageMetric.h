/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __itkAdvancedNormalizedCorrelationImageToImageMetric_h
#define __itkAdvancedNormalizedCorrelationImageToImageMetric_h

#include "itkAdvancedImageToImageMetric.h"


namespace itk
{
/** \class AdvancedNormalizedCorrelationImageToImageMetric
 * \brief Computes normalized correlation between two images, based on AdvancedImageToImageMetric...
 *
 * This metric computes the correlation between pixels in the fixed image
 * and pixels in the moving image. The spatial correspondance between
 * fixed and moving image is established through a Transform. Pixel values are
 * taken from the fixed image, their positions are mapped to the moving
 * image and result in general in non-grid position on it. Values at these
 * non-grid position of the moving image are interpolated using a user-selected
 * Interpolator. The correlation is normalized by the autocorrelations of both
 * the fixed and moving images.
 *
 * This implementation of the NormalizedCorrelation is based on the
 * AdvancedImageToImageMetric, which means that:
 * \li It uses the ImageSampler-framework
 * \li It makes use of the compact support of B-splines, in case of B-spline transforms.
 * \li Image derivatives are computed using either the B-spline interpolator's implementation
 * or by nearest neighbor interpolation of a precomputed central difference image.
 * \li A minimum number of samples that should map within the moving image (mask) can be specified.
 *
 * The normalized correlation NC is defined as:
 *
 * \f[
 * \mathrm{NC} = \frac{\sum_x f(x) * m(x+u(x,p))}{\sqrt{ \sum_x f(x)^2 * \sum_x m(x+u(x,p))^2}}
 *    = \frac{\mathtt{sfm}}{\sqrt{\mathtt{sff} * \mathtt{smm}}}
 * \f]
 *
 * where x a voxel in the fixed image f, m the moving image, u(x,p) the
 * deformation of x depending on the transform parameters p. sfm, sff and smm
 * is notation used in the source code. The derivative of NC to p equals:
 *
 * \f[
 *   \frac{\partial \mathrm{NC}}{\partial p} = \frac{\partial \mathrm{NC}}{\partial m} \frac{\partial m}{\partial x} \frac{\partial x}{\partial p} = \frac{\partial \mathrm{NC}}{\partial m} * \mathtt{gradient} * \mathtt{jacobian},
 * \f]
 *
 * where gradient is the derivative of the moving image m to x, and where Jacobian is the
 * derivative of the transformation to its parameters. gradient * Jacobian is called the differential.
 * This yields for the derivative:
 *
 * \f[
 *   \frac{\partial \mathrm{NC}}{\partial p} = \frac{\sum_x[ f(x) * \mathtt{differential} ] - ( \mathtt{sfm} / \mathtt{smm} ) * \sum_x[ m(x+u(x,p)) * \mathtt{differential} ]}{\sqrt{\mathtt{sff} * \mathtt{smm}}}
 * \f]
 *
 * This class has an option to subtract the sample mean from the sample values
 * in the cross correlation formula. This typically results in narrower valleys
 * in the cost fucntion NC. The default value is false. If SubtractMean is true,
 * the NC is defined as:
 *
 * \f[
 * \mathrm{NC} = \frac{\sum_x ( f(x) - \mathtt{Af} ) * ( m(x+u(x,p)) - \mathtt{Am})}{\sqrt{\sum_x (f(x) - \mathtt{Af})^2 * \sum_x (m(x+u(x,p)) - \mathtt{Am})^2}}
 *    = \frac{\mathtt{sfm} - \mathtt{sf} * \mathtt{sm} / N}{\sqrt{(\mathtt{sff} - \mathtt{sf} * \mathtt{sf} / N) * (\mathtt{smm} - \mathtt{sm} *\mathtt{sm} / N)}},
 * \f]
 *
 * where Af and Am are the average of f and m, respectively.
 *
 *
 * \ingroup RegistrationMetrics
 * \ingroup Metrics
 */
template < class TFixedImage, class TMovingImage >
class AdvancedNormalizedCorrelationImageToImageMetric :
    public AdvancedImageToImageMetric< TFixedImage, TMovingImage >
{
public:

  /** Standard class typedefs. */
  typedef AdvancedNormalizedCorrelationImageToImageMetric     Self;
  typedef AdvancedImageToImageMetric<
    TFixedImage, TMovingImage >                         Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( AdvancedNormalizedCorrelationImageToImageMetric, AdvancedImageToImageMetric );

  /** Typedefs from the superclass. */
  typedef typename
    Superclass::CoordinateRepresentationType              CoordinateRepresentationType;
  typedef typename Superclass::MovingImageType            MovingImageType;
  typedef typename Superclass::MovingImagePixelType       MovingImagePixelType;
  typedef typename Superclass::MovingImageConstPointer    MovingImageConstPointer;
  typedef typename Superclass::FixedImageType             FixedImageType;
  typedef typename Superclass::FixedImageConstPointer     FixedImageConstPointer;
  typedef typename Superclass::FixedImageRegionType       FixedImageRegionType;
  typedef typename Superclass::TransformType              TransformType;
  typedef typename Superclass::TransformPointer           TransformPointer;
  typedef typename Superclass::InputPointType             InputPointType;
  typedef typename Superclass::OutputPointType            OutputPointType;
  typedef typename Superclass::TransformParametersType    TransformParametersType;
  typedef typename Superclass::TransformJacobianType      TransformJacobianType;
  typedef typename Superclass::InterpolatorType           InterpolatorType;
  typedef typename Superclass::InterpolatorPointer        InterpolatorPointer;
  typedef typename Superclass::RealType                   RealType;
  typedef typename Superclass::GradientPixelType          GradientPixelType;
  typedef typename Superclass::GradientImageType          GradientImageType;
  typedef typename Superclass::GradientImagePointer       GradientImagePointer;
  typedef typename Superclass::GradientImageFilterType    GradientImageFilterType;
  typedef typename Superclass::GradientImageFilterPointer GradientImageFilterPointer;
  typedef typename Superclass::FixedImageMaskType         FixedImageMaskType;
  typedef typename Superclass::FixedImageMaskPointer      FixedImageMaskPointer;
  typedef typename Superclass::MovingImageMaskType        MovingImageMaskType;
  typedef typename Superclass::MovingImageMaskPointer     MovingImageMaskPointer;
  typedef typename Superclass::MeasureType                MeasureType;
  typedef typename Superclass::DerivativeType             DerivativeType;
  typedef typename Superclass::ParametersType             ParametersType;
  typedef typename Superclass::FixedImagePixelType        FixedImagePixelType;
  typedef typename Superclass::MovingImageRegionType      MovingImageRegionType;
  typedef typename Superclass::ImageSamplerType           ImageSamplerType;
  typedef typename Superclass::ImageSamplerPointer        ImageSamplerPointer;
  typedef typename Superclass::ImageSampleContainerType   ImageSampleContainerType;
  typedef typename
    Superclass::ImageSampleContainerPointer               ImageSampleContainerPointer;
  typedef typename Superclass::FixedImageLimiterType      FixedImageLimiterType;
  typedef typename Superclass::MovingImageLimiterType     MovingImageLimiterType;
  typedef typename
    Superclass::FixedImageLimiterOutputType               FixedImageLimiterOutputType;
  typedef typename
    Superclass::MovingImageLimiterOutputType              MovingImageLimiterOutputType;
  typedef typename
    Superclass::MovingImageDerivativeScalesType           MovingImageDerivativeScalesType;

  /** The fixed image dimension. */
  itkStaticConstMacro( FixedImageDimension, unsigned int,
    FixedImageType::ImageDimension );

  /** The moving image dimension. */
  itkStaticConstMacro( MovingImageDimension, unsigned int,
    MovingImageType::ImageDimension );

  /** Get the value for single valued optimizers. */
  MeasureType GetValue( const TransformParametersType & parameters ) const;

  /** Get the derivatives of the match measure. */
  void GetDerivative( const TransformParametersType & parameters,
    DerivativeType & Derivative ) const;

  /** Get value and derivatives for multiple valued optimizers. */
  void GetValueAndDerivative( const TransformParametersType & parameters,
    MeasureType& Value, DerivativeType& Derivative ) const;

  /** Set/Get SubtractMean boolean. If true, the sample mean is subtracted
   * from the sample values in the cross-correlation formula and
   * typically results in narrower valleys in the cost fucntion.
   * Default value is false. */
  itkSetMacro( SubtractMean, bool );
  itkGetConstReferenceMacro( SubtractMean, bool );
  itkBooleanMacro( SubtractMean );

protected:
  AdvancedNormalizedCorrelationImageToImageMetric();
  virtual ~AdvancedNormalizedCorrelationImageToImageMetric() {};
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Protected Typedefs ******************/

  /** Typedefs inherited from superclass */
  typedef typename Superclass::FixedImageIndexType                FixedImageIndexType;
  typedef typename Superclass::FixedImageIndexValueType           FixedImageIndexValueType;
  typedef typename Superclass::MovingImageIndexType               MovingImageIndexType;
  typedef typename Superclass::FixedImagePointType                FixedImagePointType;
  typedef typename Superclass::MovingImagePointType               MovingImagePointType;
  typedef typename Superclass::MovingImageContinuousIndexType     MovingImageContinuousIndexType;
  typedef typename Superclass::BSplineInterpolatorType            BSplineInterpolatorType;
  typedef typename Superclass::CentralDifferenceGradientFilterType CentralDifferenceGradientFilterType;
  typedef typename Superclass::MovingImageDerivativeType          MovingImageDerivativeType;
  typedef typename Superclass::NonZeroJacobianIndicesType         NonZeroJacobianIndicesType;

  /** Computes the innerproduct of transform Jacobian with moving image gradient.
   * The results are stored in imageJacobian, which is supposed
   * to have the right size (same length as Jacobian's number of columns). */
  void EvaluateTransformJacobianInnerProduct(
    const TransformJacobianType & jacobian,
    const MovingImageDerivativeType & movingImageDerivative,
    DerivativeType & imageJacobian) const;

  /** Compute a pixel's contribution to the derivative terms;
   * Called by GetValueAndDerivative(). */
  void UpdateDerivativeTerms(
    const RealType fixedImageValue,
    const RealType movingImageValue,
    const DerivativeType & imageJacobian,
    const NonZeroJacobianIndicesType & nzji,
    DerivativeType & derivativeF,
    DerivativeType & derivativeM,
    DerivativeType & differential ) const;

private:
  AdvancedNormalizedCorrelationImageToImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool    m_SubtractMean;

}; // end class AdvancedNormalizedCorrelationImageToImageMetric

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAdvancedNormalizedCorrelationImageToImageMetric.txx"
#endif

#endif // end #ifndef __itkAdvancedNormalizedCorrelationImageToImageMetric_h

