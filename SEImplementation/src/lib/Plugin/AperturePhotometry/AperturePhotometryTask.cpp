/*
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include <iostream>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"

namespace SExtractor {

namespace {
  // enhancing from 5 to 10 smoothens the photometry
  const int SUPERSAMPLE_NB = 10;
  const SeFloat CROWD_THRESHOLD_APER   = 0.1;
  const SeFloat BADAREA_THRESHOLD_APER = 0.1;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryTask::computeProperties(SourceInterface& source) const {
  // get the detection frame
  const auto& detection_frame  = source.getProperty<DetectionFrame>().getFrame();

  // get the images and image information from the frame
  const auto& detection_image    = detection_frame->getSubtractedImage();
  const auto& detection_variance = detection_frame->getVarianceMap();
  const auto& variance_threshold = detection_frame->getVarianceThreshold();
  const auto& threshold_image    = detection_frame->getThresholdedImage();

  // get the object center
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

  // get the pixel list
  const auto& pix_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  // get the aperture borders on the image
  auto min_pixel = m_aperture->getMinPixel(centroid_x, centroid_y);
  auto max_pixel = m_aperture->getMaxPixel(centroid_x, centroid_y);

  // get the neighbourhood information
  NeighbourInfo neighbour_info(min_pixel, max_pixel, pix_list, threshold_image);

  SeFloat  total_flux     = 0;
  SeFloat  total_variance = 0.0;
  long int total_flag           = 0;
  SeFloat total_area = 0.0;
  SeFloat bad_area=0;
  SeFloat full_area=0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat pixel_value    = 0;
      SeFloat pixel_variance = 0;
      SeFloat variance_tmp   = 0;

      // get the area coverage and continue if there is overlap
      auto area = m_aperture->getArea(centroid_x, centroid_y, pixel_x, pixel_y);
      if (area>0){

        // make sure the pixel is inside the image
        if (pixel_x >=0 && pixel_y >=0 && pixel_x < detection_image->getWidth() && pixel_y < detection_image->getHeight()) {

          // enhance the area
          total_area += area;


          variance_tmp = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;
          if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y) || variance_tmp > variance_threshold) {

            // enhance the area affected by a defect
            if (neighbour_info.isNeighbourObjectPixel(pixel_x, pixel_y))
              full_area+=1;
            if (variance_tmp > variance_threshold)
              bad_area+=1;

            // try using the mirror pixel
            if (m_use_symmetry){
              // get the mirror pixel
              auto mirror_x = 2 * centroid_x - pixel_x + 0.49999;
              auto mirror_y = 2 * centroid_y - pixel_y + 0.49999;
              if (mirror_x >=0 && mirror_y >=0 && mirror_x < detection_image->getWidth() && mirror_y < detection_image->getHeight()) {
                variance_tmp = detection_variance ? detection_variance->getValue(mirror_x, mirror_y) : 1;
                if (!neighbour_info.isNeighbourObjectPixel(mirror_x, mirror_y) && variance_tmp < variance_threshold) {
                  // mirror pixel is OK: take the value
                  pixel_value    = detection_image->getValue(mirror_x, mirror_y);
                  pixel_variance = variance_tmp;
                }
              }
            }
          }
          else {
            pixel_value    = detection_image->getValue(pixel_x, pixel_y);
            pixel_variance = variance_tmp;
          }
          total_flux     += pixel_value * area;
          total_variance += pixel_variance * area;
        }
        else{
          total_flag |= 0x0008;
        }
      }
    }
  }

  if (total_area>0){
    // check/set the bad area flag
    if ((SeFloat)bad_area/(SeFloat)total_area > BADAREA_THRESHOLD_APER)
      total_flag |= 0x0001;

    // check/set the crowded area flag
    if ((SeFloat)full_area/(SeFloat)total_area > BADAREA_THRESHOLD_APER)
      total_flag |= 0x0002;
  }

  // compute the derived quantities
  auto flux_error = sqrt(total_variance);
  auto mag = total_flux > 0.0 ? -2.5*log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
  auto mag_error = 1.0857 * flux_error / total_flux;

  // set the source properties
  source.setIndexedProperty<AperturePhotometry>(m_instance, total_flux, flux_error, mag, mag_error, total_flag);
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryAggregateTask::computeProperties(SourceInterface& source) const {
  SeFloat flux = 0;
  for (auto instance : m_instances_to_aggregate) {
    auto aperture_photometry = source.getProperty<AperturePhotometry>(instance);
    flux += aperture_photometry.getFlux();
  }
  flux /= m_instances_to_aggregate.size();

  auto mag = flux > 0.0 ? -2.5*log10(flux) + m_magnitude_zero_point : SeFloat(99.0);
  source.setIndexedProperty<AperturePhotometry>(m_instance, flux, 999999, mag, 999999, 0); // FIXME
}

//////////////////////////////////////////////////////////////////////////////////////////

SeFloat CircularAperture::getArea(SeFloat center_x, SeFloat center_y, int pixel_x, int pixel_y) const {
  auto dx = pixel_x - center_x;
  auto dy = pixel_y - center_y;
  SeFloat min_supersampled_radius_squared = m_radius > .75 ? (m_radius - .75) * (m_radius - .75) : 0;
  SeFloat max_supersampled_radius_squared = (m_radius + .75) * (m_radius + .75);

  auto distance_squared = dx * dx + dy * dy;
  if (distance_squared <= max_supersampled_radius_squared) {
    if (distance_squared < min_supersampled_radius_squared) {
      return 1.0;
    } else {
      SeFloat area = 0.0;
      for (int sub_y = 0; sub_y < SUPERSAMPLE_NB; sub_y++) {
        for (int sub_x = 0; sub_x < SUPERSAMPLE_NB; sub_x++) {
          auto dx2 = dx + SeFloat(sub_x - SUPERSAMPLE_NB/2) / SUPERSAMPLE_NB;
          auto dy2 = dy + SeFloat(sub_y - SUPERSAMPLE_NB/2) / SUPERSAMPLE_NB;
          auto supersampled_distance_squared = dx2 * dx2 + dy2 * dy2;
          if (supersampled_distance_squared <= m_radius * m_radius) {
            area += 1.0 / (SUPERSAMPLE_NB * SUPERSAMPLE_NB);
          }
        }
      }
      return area;
    }
  }
  return 0.0;
}

PixelCoordinate CircularAperture::getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const {
  return PixelCoordinate(centroid_x - m_radius, centroid_y - m_radius);
}

PixelCoordinate CircularAperture::getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const {
  return PixelCoordinate(centroid_x + m_radius + 1, centroid_y + m_radius + 1);
}

}

